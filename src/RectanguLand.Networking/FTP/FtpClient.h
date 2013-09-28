#pragma once

#include "../Socket.h"

namespace Network
{
	namespace Ftp
	{
		SHARED_FORWARD(FtpFileSystemEntry);
		SHARED_FORWARD(FtpFileEntry);
		SHARED_FORWARD(FtpDirectoryEntry);

		class FtpFileSystemEntry
		{
		protected:
			String mEntryName;
		public:
			FtpFileSystemEntry(const String& name) {
				mEntryName = name;
			}

			virtual String getEntryName() const { return mEntryName; }
			virtual bool isDirectory() const = 0;
		};

		class FtpFileEntry : public FtpFileSystemEntry
		{
		public:
			FtpFileEntry(const String& name) : FtpFileSystemEntry(name) {

			}

			bool isDirectory() const { return false; }
		};

		class FtpDirectoryEntry : public FtpFileSystemEntry
		{
			std::list<FtpDirectoryEntryPtr> mSubDirs;
			std::list<FtpFileEntryPtr> mFiles;
		public:
			FtpDirectoryEntry(const String& name) : FtpFileSystemEntry(name) {

			}

			void addEntry(FtpFileSystemEntryPtr entry) {
				if (entry->isDirectory()) {
					mSubDirs.push_back(std::dynamic_pointer_cast<FtpDirectoryEntry>(entry));
				} else {
					mFiles.push_back(std::dynamic_pointer_cast<FtpFileEntry>(entry));
				}
			}

			bool isDirectory() const { return true; }
		};		

		class FtpClient : public std::enable_shared_from_this<FtpClient>
		{
			friend class FtpFileStream;

			class ParsedCommand
			{
			private:
				uint32 mCommand;
				String mParams;
				String mFullCmd;

			public:
				ParsedCommand() : mCommand(0) { }

				ParsedCommand(const String& cmd);

				void setFullCommand(const String& cmd) { mFullCmd = cmd; }

				uint32 getCommand() const { return mCommand; }
				String getParams()const{ return mParams; }
				String getFullCommand() const { return mFullCmd; }
			};

			SocketPtr mSocket;
			bool mIsAuthenticated;
			std::mutex mCommandLock;
			std::mutex mConditionLock;
			std::condition_variable mCommandCondition;
			bool mIsBusy;

			FtpDirectoryEntryPtr mFileRoot;

			template<uint32 code>
			ParsedCommand assertCode() {
				ParsedCommand cmd;
				receiveCommand(cmd);

				assert(code == cmd.getCommand());

				return cmd;
			}

			void receiveCommand(ParsedCommand& cmd);
			void readBanner(ParsedCommand& cmd, String& banner);
			void parseMultiLineCommand(ParsedCommand& cmd);

			void informReadComplete();
			void loadFileSystem();
			void loadDirectory(const String& curPath, FtpDirectoryEntryPtr dir);
			FtpFileSystemEntryPtr createEntry(const std::unordered_map<String, String, String::StrHash>& propMap);

			SocketPtr enterPasv();

		public:
			FtpClient(const String& host, uint16 port = 21);

			bool tryLogin(const String& username, const String& password);
			bool saveFile(const String& remote, const String& local);
			bool retreiveFile(const String& file, std::shared_ptr<std::streambuf>& outBuff);
			void listDirectoryFiles(const String& dir);
		};

		SHARED_TYPE(FtpClient);
		WEAK_TYPE(FtpClient);
	}
}