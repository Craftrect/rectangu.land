#pragma once

namespace IO
{
	SHARED_FORWARD(BindingCommand);

	class BindingCommand
	{
	private:
		std::function<void (BindingCommandPtr)> mCallback;

	public:
		BindingCommand(std::function<void (BindingCommandPtr)> callback) : mCallback(callback) {

		}
	};

	class BindingManager : public Singleton<BindingManager>
	{
		std::map<uint32, BindingCommandPtr> mWellKnownActions;
		std::map<uint32, BindingCommandPtr> mCustomActions;

	public:
		BindingManager();

	};
}

#define sBindingMgr (IO::BindingManager::getInstance())