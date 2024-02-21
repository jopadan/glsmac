#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "FS.h"

using namespace std;

#ifdef DEBUG
#define Log( _text ) std::cout << "<Util::FS> " << (_text) << std::endl
#else
#define Log( _text )
#endif

namespace util {

const char FS::PATH_SEPARATOR =
#ifdef _WIN32
	'\\'
#else
	'/'
#endif
	;

const char FS::EXTENSION_SEPARATOR = '.';

const std::string FS::NormalizePath( const std::string& path, const char path_separator ) {
	if ( path_separator == PATH_SEPARATOR ) {
		return path;
	}
	else {
		std::string result = path;
		std::replace( result.begin(), result.end(), path_separator, PATH_SEPARATOR );
		return result;
	}
}

const std::string FS::ConvertPath( const std::string& path, const char path_separator ) {
	if ( path_separator == PATH_SEPARATOR ) {
		return path;
	}
	else {
		std::string result = path;
		std::replace( result.begin(), result.end(), PATH_SEPARATOR, path_separator );
		return result;
	}
}

const std::string FS::GetUpDirString() {
	return "..";
}

const std::string FS::GeneratePath( const std::vector< std::string >& parts, const char path_separator ) {
	std::string result = "";
	for ( const auto& part : parts ) {
		if ( !result.empty() ) {
			result += path_separator;
		}
		result += part;
	}
	return result;
}

#ifdef _WIN32

const bool FS::IsWindowsDriveLabel( const std::string& directory ) {
	return
		directory.size() == 2 &&
		directory[0] >= 'A' &&
		directory[0] <= 'Z' &&
		directory[1] == ':' // C:, D:, ...
	;
	// todo: multiletter drives?
}

const std::vector< std::string > FS::GetWindowsDrives() {
	std::vector< std::string > result = {};
	const auto drives = GetLogicalDrives();
	ASSERT_NOLOG( sizeof( drives ) == 4, "drives bitmask is not 32-bit" );
	for ( uint8_t i = 0; i < ( 'Z' - 'A' ); i++ ) {
		if ( drives & ( 1 << i ) ) {
			result.push_back( std::string(1, 'A' + i) + ":" );
		}
	}
	return result;
}

#endif

bool FS::IsAbsolutePath( const std::string& path, const char path_separator ) {
	return
#ifdef _WIN32
		(  // the very root (without drive)
			path.size() == 1 &&
			path[0] == path_separator
		) ||
		(
			path.size() >= 2 &&
			path[0] >= 'A' &&
			path[0] <= 'Z' &&
			path[1] == ':' &&
			(
				path.size() == 2 ||
				(
					path.size() >= 3 &&
					path[ 2 ] == path_separator
				)
			)
		)
#else
		!path.empty() &&
			path[ 0 ] == '/'
#endif
		;
}

const std::string FS::GetCurrentDirectory( const char path_separator ) {
	return ConvertPath( std::filesystem::current_path().string(), path_separator );
}

const std::string FS::GetDirName( const std::string& path, const char path_separator ) {
	const auto pos = path.rfind( path_separator );
	if ( pos == std::string::npos ) {
		return "";
	}
	else {
		return path.substr( 0, pos );
	}
}

const std::string FS::GetBaseName( const std::string& path, const char path_separator ) {
	const auto pos = path.rfind( path_separator );
	if ( pos == std::string::npos ) {
		return path;
	}
	else {
		return path.substr( pos + 1 );
	}
}

const std::string FS::GetFilteredPath( const std::string& path, const char path_separator ) {
	size_t pos = 0;
	if ( path == "." ) {
		return "";
	}
	while ( pos + 1 < path.size() && path[ pos ] == EXTENSION_SEPARATOR && path[ pos + 1 ] == path_separator ) {
		pos += 2;
	}
	return pos
		? path.substr( pos )
		: path;
}

const std::string FS::GetAbsolutePath( const std::string& path, const char path_separator ) {
	return IsAbsolutePath( path )
		? path
		: GetCurrentDirectory() + path_separator + GetFilteredPath( path );
}

const std::string FS::GetExtension( const std::string& path, const char path_separator ) {
	const auto pos = path.rfind( EXTENSION_SEPARATOR );
	if ( pos == std::string::npos ) {
		return "";
	}
	else {
		const auto pathsep_pos = path.rfind( path_separator );
		if ( pathsep_pos > pos ) {
			return "";
		}
		return path.substr( pos );
	}
}

const std::vector< std::string > FS::GetExtensions( const std::string& path, const char path_separator ) {
	std::vector< std::string > result = {};
	size_t pos, path_pos, last_pos = path.size();
	while ( ( pos = path.rfind( EXTENSION_SEPARATOR, last_pos - 1 ) ) != std::string::npos ) {
		path_pos = path.rfind( path_separator, last_pos - 1 );
		if ( path_pos != std::string::npos && path_pos > pos ) {
			break;
		}
		result.push_back( path.substr( pos, last_pos - pos ) );
		last_pos = pos;
	}
	std::reverse( result.begin(), result.end() );
	return result;
}

const bool FS::Exists( const string& path, const char path_separator ) {
	return std::filesystem::exists( NormalizePath( path, path_separator ) );
}

const bool FS::IsFile( const string& path, const char path_separator ) {
	return std::filesystem::is_regular_file( NormalizePath( path, path_separator ) );
}

const bool FS::FileExists( const string& path, const char path_separator ) {
	return Exists( path, path_separator ) && IsFile( path, path_separator );
}

const bool FS::IsDirectory( const string& path, const char path_separator ) {
	return std::filesystem::is_directory( NormalizePath( path, path_separator ) );
}

const bool FS::DirectoryExists( const string& path, const char path_separator ) {
	return Exists( path, path_separator ) && IsDirectory( path, path_separator );
}

void FS::CreateDirectoryIfNotExists( const string& path, const char path_separator ) {
	if ( !DirectoryExists( path, path_separator ) ) {
		//Log( "Creating directory: " + path );
		std::filesystem::create_directories( NormalizePath( path, path_separator ) );
	}
}

#ifdef _WIN32
std::vector< std::string > FS::ListDirectory( std::string directory, const bool return_absolute_paths, const char path_separator ) {
	ASSERT_NOLOG( !directory.empty(), "directory is empty" );
#else

std::vector< std::string > FS::ListDirectory( const std::string& directory, const bool return_absolute_paths ) {
#endif
	std::vector< std::string > result = {};

	//Log( "Reading directory: " + directory );

	try {

#ifdef _WIN32
		const bool is_windows_drive_label = IsWindowsDriveLabel( directory );
		if ( is_windows_drive_label ) {
			directory += path_separator;
		}
#endif

		std::vector< std::filesystem::path > items;
		std::copy(
			std::filesystem::directory_iterator(
				!directory.empty()
					? directory
					: std::string( 1, path_separator )
			),
			std::filesystem::directory_iterator(),
			std::back_inserter( items )
		);
		std::sort( items.begin(), items.end() );

		for ( const auto& item : items ) {
			const auto item_str = item.string();
			const uint8_t prefix_len = directory[ directory.size() - 1 ] == path_separator
				? 0
				: 1;
			ASSERT_NOLOG( item_str.substr( 0, directory.size() ) == directory, "unexpected path in directory list results: " + item_str );
			result.push_back( ConvertPath(
				return_absolute_paths
					? item_str
					: item_str.substr( directory.size() + prefix_len )
			, path_separator ) );
		}
	}
	catch ( std::filesystem::filesystem_error& e ) {
		// permission denied etc
		// TODO: display error?
		return result;
	}

	return result;
}

const string FS::ReadFile( const string& path, const char path_separator ) {
	//Log( "Reading file: " + path );
	ASSERT_NOLOG( FileExists( path, path_separator ), "file \"" + path + "\" does not exist or is not a file" );
	stringstream data;
	ifstream in( NormalizePath( path, path_separator ), std::ios_base::binary );
	while ( data << in.rdbuf() ) {}
	in.close();
	return data.str();
}

const void FS::WriteFile( const string& path, const string& data, const char path_separator ) {
	//Log( "Writing file: " + path );
	ofstream out( NormalizePath( path, path_separator ), std::ios_base::binary );
	out << data;
	out.close();
}

}

