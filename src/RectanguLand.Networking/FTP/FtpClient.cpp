#include "Precompiled.h"
#include "FtpClient.h"
#include "FtpFileStream.h"

namespace Network
{
	namespace Ftp
	{
		FtpClient::FtpClient(const String& host, uint16 port) : mIsAuthenticated(false), mIsBusy(false) {
			mSocket = std::make_shared<Socket>();

			mSocket->connect(host, port);

			ParsedCommand hello;
			String banner;

			readBanner(hello, banner);

			mIsBusy = false;
		}

		bool FtpClient::tryLogin(const String& username, const String& password) {
			if (mIsAuthenticated == true) {
				return true;
			}

			auto& out = mSocket->getOutputStream();
			out << "USER " << username.toMultibyte() << "\r" << std::endl;

			ParsedCommand resp;
			receiveCommand(resp);
			if (resp.getCommand() != 331) {
				return false;
			}

			out << "PASS " << password.toMultibyte() << "\r" << std::endl;

			receiveCommand(resp);
			if (resp.getCommand() != 230) {
				return false;
			}

			loadFileSystem();

			return true;
		}

		bool FtpClient::saveFile(const String& remote, const String& local) {
			{
				std::unique_lock<std::mutex> l(mCommandLock);
				while (mIsBusy) {
					mCommandCondition.wait(l);
				}

				mIsBusy = true;
			}
			
			auto& out = mSocket->getOutputStream();
			out << "TYPE I\r" << std::endl;

			ParsedCommand resp;
			receiveCommand(resp);
			if (resp.getCommand() != 200) {
				return false;
			}

			auto sock = enterPasv();

			out << "RETR " << remote << "\r" << std::endl;
			receiveCommand(resp);

			std::vector<char> content;
			sock->readToEnd(content);

			receiveCommand(resp);

			if (resp.getCommand() == 226) {
				std::ofstream outFile(local.toMultibyte(), std::ios::out | std::ios::binary);
				outFile.write(content.data(), content.size());
				outFile.close();
			}

			mIsBusy = false;
			mCommandCondition.notify_all();

			return true;
		}

		SocketPtr FtpClient::enterPasv() {
			auto& out = mSocket->getOutputStream();
			out << "PASV\r" << std::endl;

			ParsedCommand resp;
			receiveCommand(resp);

			if (resp.getCommand() != 227) {
				return nullptr;
			}

			String cmd = resp.getParams();
			auto itr = std::find_if(std::begin(cmd), std::end(cmd), isdigit);
			if (itr == std::end(cmd)) {
				return nullptr;
			}

			cmd = cmd.substring(itr);
			std::list<String> parts;
			cmd.split(parts, L',');
			if (parts.size() < 6) {
				return nullptr;
			}

			std::wstringstream ipstrm;
			auto ipitr = parts.begin();
			String ip1 = (*ipitr++);
			String ip2 = (*ipitr++);
			String ip3 = (*ipitr++);
			String ip4 = (*ipitr++);
			String ip5 = (*ipitr++);
			String ip6 = (*ipitr++);

			ipstrm << ip1 << L"." << ip2 << L"." << ip3 << L"." << ip4;
			String serverIp = ipstrm.str();

			ipstrm = std::wstringstream();
			ipstrm << ip5 << L" " << ip6;
			uint16 port1, port2;
			if (!(ipstrm >> port1 >> port2)) {
				return nullptr;
			}

			uint16 port = port1 * 256 + port2;

			SocketPtr sock = std::make_shared<Socket>();
			sock->connect(serverIp, port);
			return sock;
		}

		void FtpClient::readBanner(ParsedCommand& cmd, String& banner) {
			do {
				receiveCommand(cmd);
				auto bannerl = cmd.getFullCommand();
				
				std::wstring::iterator itr = std::find_if_not(begin(bannerl), end(bannerl), std::isdigit);
				if (itr != end(bannerl) && (*itr) == L' ') {
					std::wstring numPart = bannerl->substr(0, itr - begin(bannerl));
					std::wstringstream strm;
					strm << numPart;
					uint32 code = 0;
					if (strm >> code) {
						return;
					}
				}

				banner = banner + cmd.getFullCommand() + "\n";
			} while (true);
		}

		void FtpClient::receiveCommand(ParsedCommand& cmd) {
			std::string resp;
			bool hasCR = false;
			while (true) {
				char c = mSocket->getInputStream().get();
				if (c == '\r') {
					hasCR = true;
					continue;
				}

				if (hasCR && c == '\n') {
					break;
				}

				resp += c;
			}

			cmd = ParsedCommand(resp);
		}

		FtpClient::ParsedCommand::ParsedCommand(const String& cmd) {
			mFullCmd = cmd;

			std::wstring ws = cmd;
			std::wstringstream strm;
			strm << ws;

			strm >> mCommand;
			std::getline(strm, (std::wstring&)mParams);
		}

		bool FtpClient::retreiveFile(const String& cmd, std::shared_ptr<std::streambuf>& buff) {
			{
				std::unique_lock<std::mutex> l(mCommandLock);
				while (mIsBusy) {
					mCommandCondition.wait(l);
				}

				mIsBusy = true;
			}

			auto& out = mSocket->getOutputStream();
			out << "TYPE I\r" << std::endl;

			ParsedCommand resp;
			receiveCommand(resp);
			if (resp.getCommand() != 200) {
				mCommandLock.unlock();
				return false;
			}

			auto sock = enterPasv();
			if (sock == nullptr) {
				mCommandLock.unlock();
				return false;
			}

			out << "RETR " << cmd << "\r" << std::endl;
			receiveCommand(resp);

			if (resp.getCommand() != 150) {
				mCommandLock.unlock();
				return false;
			}

			std::shared_ptr<FtpFileStream> strm = std::make_shared<FtpFileStream>(shared_from_this(), sock);

			if (strm->beginRead() == false) {
				return false;
			}

			buff = strm;
			return true;
		}

		void FtpClient::informReadComplete() {
			ParsedCommand resp;
			receiveCommand(resp);

			mIsBusy = false;
			mCommandCondition.notify_all();
		}

		void FtpClient::parseMultiLineCommand(ParsedCommand& cmd) {
			String verbose;

			do {
				receiveCommand(cmd);
				auto bannerl = cmd.getFullCommand();

				std::wstring::iterator itr = std::find_if_not(begin(bannerl), end(bannerl), std::isdigit);
				if (itr != end(bannerl) && (*itr) == L' ') {
					std::wstring numPart = bannerl->substr(0, itr - begin(bannerl));
					std::wstringstream strm;
					strm << numPart;
					uint32 code = 0;
					if (strm >> code) {
						cmd.setFullCommand(verbose);
						return;
					}
				}

				verbose = verbose + cmd.getFullCommand() + "\n";
			} while (true);
		}

		void FtpClient::listDirectoryFiles(const String& dir) {
			{
				std::unique_lock<std::mutex> l(mCommandLock);
				while (mIsBusy) {
					mCommandCondition.wait(l);
				}

				mIsBusy = true;
			}

			auto sock = enterPasv();
			if (sock == nullptr) {
				mIsBusy = false;
				mCommandCondition.notify_all();
				return;
			}

			auto& out = mSocket->getOutputStream();
			out << "MLSD ";
			if (dir.empty() == false)
				out << dir;
			out << "\r" << std::endl;

			ParsedCommand cmd;
			parseMultiLineCommand(cmd);

			std::vector<char> data;
			sock->readToEnd(data);
			data.push_back(0);
			String str = data.data();
			std::list<String> entries;
			str.split(entries, L'\r');

			for (auto& elem : entries) {
				if (elem.startsWith("\n")) {
					elem = elem.substring(1);
				}

				if (elem.empty()) {
					continue;
				}

				std::list<String> properties;
				elem.split(properties, L';');

				String name = properties.back().trim();
			}

			mCommandCondition.notify_all();
			mIsBusy = false;
		}

		void FtpClient::loadFileSystem() {
			{
				std::unique_lock<std::mutex> l(mCommandLock);
				while (mIsBusy) {
					mCommandCondition.wait(l);
				}

				mIsBusy = true;
			}

			mFileRoot = std::make_shared<FtpDirectoryEntry>("");

			loadDirectory("", mFileRoot);

			mCommandCondition.notify_all();
			mIsBusy = false;
		}

		void FtpClient::loadDirectory(const String& path, FtpDirectoryEntryPtr dir) {
			String curDir = path;
			curDir += "/";
			curDir += dir->getEntryName();

			auto sock = enterPasv();
			if (sock == nullptr) {
				return;
			}

			auto& out = mSocket->getOutputStream();
			out << "MLSD ";
			if (curDir.empty() == false)
				out << curDir;
			out << "\r" << std::endl;

			ParsedCommand cmd;
			parseMultiLineCommand(cmd);

			std::vector<char> data;
			sock->readToEnd(data);
			data.push_back(0);

			receiveCommand(cmd);

			String str = data.data();
			std::list<String> entries;
			str.split(entries, L'\r');

			for (auto& elem : entries) {
				if (elem.startsWith("\n")) {
					elem = elem.substring(1);
				}

				if (elem.empty()) {
					continue;
				}

				std::list<String> properties;
				elem.split(properties, L';');

				String name = properties.back().trim();
				if (name == "." || name == "..") {
					continue;
				}

				properties.pop_back();

				std::unordered_map<String, String, String::StrHash> propMap;
				propMap["name"] = name;
				for (auto& prop : properties) {
					if (prop.find(L'=') == false) {
						continue;
					}

					std::list<String> kvp;
					prop.toLower().split(kvp, L'=');
					if (kvp.size() != 2) {
						continue;
					}

					propMap[kvp.front()] = kvp.back();
				}

				auto entry = createEntry(propMap);
				if (entry == nullptr) {
					continue;
				}

				dir->addEntry(entry);

				if (entry->isDirectory()) {
					loadDirectory(curDir, std::dynamic_pointer_cast<FtpDirectoryEntry>(entry));
				}
			}
		}

		FtpFileSystemEntryPtr FtpClient::createEntry(const std::unordered_map<String, String, String::StrHash>& props) {
			auto typeItr = props.find("type");
			auto nameItr = props.find("name");

			if (typeItr == props.end() || nameItr == props.end()) {
				return nullptr;
			}

			String name = nameItr->second;
			if (typeItr->second == "dir") {
				return std::make_shared<FtpDirectoryEntry>(name);
			} else if (typeItr->second == "file") {
				return std::make_shared<FtpFileEntry>(name);
			} else {
				return nullptr;
			}
		}
	}
}