#pragma once
#include <iostream>
#include <mutex>

#ifdef _DEBUG
#define ASSERT(expr) \
	do { \
		if(!(expr)){ \
			std::cout << "========================================\n"; \
			std::cout << "[ASSERTION FAILED!]\n"; \
			std::cout << "Condition : " << #expr << "\n"; \
			std::cout << "File : " << __FILE__ << "\n"; \
			std::cout << "Line : " << __LINE__ << "\n"; \
			std::cout << "========================================\n"; \
			__debugbreak(); \
		} \
	} while(0)

#else
#define ASSERT(expr) ((void)0)

#endif // DEBUG

#define ASSERT_CRASH(expr) \
{ \
	if(!(expr)) \
	{ \
		__debugbreak(); \
	} \
}

#define USE_LOCK				std::mutex _lock;
#define USE_MANY_LOCKS(count)	std::mutex _locks[count];

#define USE_STATIC_LOCK			static std::mutex _lock;
								
#define WRITE_LOCK				std::lock_guard<std::mutex> lockGuard(_lock);
#define WRITE_LOCK_IDX(idx)		std::lock_guard<std::mutex> lockGuard(_locks[idx]);