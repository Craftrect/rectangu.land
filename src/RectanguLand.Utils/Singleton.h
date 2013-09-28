#pragma once

template<typename T>
class Singleton
{
private:
	static std::shared_ptr<T> gInstance;

public:
	static std::shared_ptr<T> getInstance() {
		if (gInstance == nullptr) {
			gInstance = std::make_shared<T>();
		}

		return gInstance;
	}
};

#define INIT_SINGLETON(type) std::shared_ptr<type> Singleton<type>::gInstance;