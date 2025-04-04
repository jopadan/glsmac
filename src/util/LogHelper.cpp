#include "LogHelper.h"

#include <mutex>
#include <iostream>

namespace util {

static std::mutex s_cout_mutex;

void LogHelper::Print( const std::string& text ) {
	std::lock_guard guard( s_cout_mutex );
	std::cout << text;
}

void LogHelper::Println( const std::string& text ) {
	std::lock_guard guard( s_cout_mutex );
	std::cout << text << std::endl;
}

void LogHelper::Flush() {
	std::lock_guard guard( s_cout_mutex );
	std::cout << std::flush;
}

}
