#pragma once

#include <string>

#include "base/Module.h"

using namespace std;

namespace config {

MAJOR_CLASS( Config, base::Module )
	Config( const int argc, const char *argv[] );
	
	const string& GetGamePath() {
		return m_game_path;
	}
	
private:
	const int m_argc = 0;
	const char** const m_argv = nullptr;
	
	string m_game_path = "";
};

} /* namespace config */
