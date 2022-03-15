/**
 * Utility module for geting information and paths from filesystem
*/

#include "filesystem.hpp"

#if __has_include(<windows.h>)
//MSWIN system
#define MSWIN_SYSTEM
//#define NTDDI_VERSION 0x06000000
#include <windows.h>
#include <wchar.h>
#include <initguid.h>
#include <KnownFolders.h>
#include <Shlobj.h>
#include <string.h>

//funcions for conversion from/to wstrings

///converts from WideString to UTF-8 string
std::string fromWString(wchar_t *wideString, DWORD wideSize) {
	char *resultChar = nullptr;
	int requiredSize = WideCharToMultiByte( CP_UTF8, 0, wideString, wideSize, resultChar, 0, NULL, NULL);
	if( requiredSize > 0 ) {
		resultChar = new char[requiredSize];
		if( WideCharToMultiByte( CP_UTF8, 0, wideString, wideSize, resultChar, requiredSize, NULL, NULL) == 0 ) {//conversion error?
			delete[] resultChar;
			return "";
		} else { //success?
			std::string result = std::string(resultChar, requiredSize);
			delete[] resultChar;
			return result;
		}
	} else { //conversion error
		return "";
	}
}

///converts from UTF-8 string to WideString
std::wstring toWString(std::string source) {
	std::wstring resultWString;
	int requiredSize = MultiByteToWideChar( CP_UTF8, 0, source.c_str(), source.length(), nullptr, 0);
	if( requiredSize > 0 ) {
		wchar_t* resultWChar = new wchar_t[requiredSize];
		if( MultiByteToWideChar( CP_UTF8, 0, source.c_str(), source.length(), resultWChar, requiredSize) == 0 ) {//conversion error
			delete[] resultWChar;
			return L"";
		} else { //success?
			resultWString = std::wstring(resultWChar, requiredSize);
			delete[] resultWChar;
		}
	} else { //conversion error
		return L"";
	}
	return resultWString;
}

#else
//POSIX system
#include <limits.h>
#include <unistd.h>

#endif

#include <sys/stat.h>
#include <stdio.h>


namespace filesystem {
	
	///returns directory name of given string
	std::string dirName(std::string path) {
		std::string::size_type pos = path.find_last_of(
			#ifdef MSWIN_SYSTEM
			"\\/"
			#else
			"/"
			#endif
			);
		return (std::string::npos == pos) ? "" : path.substr(0, pos);
	}
	
	///returns the program's directory
	std::string getProgramDir() {
		#ifdef MSWIN_SYSTEM
		wchar_t result[ MAX_PATH ];
		DWORD resultSize = GetModuleFileNameW( NULL, result, MAX_PATH );
		
		return dirName( fromWString(result, resultSize) );
		#else
		char result[ PATH_MAX ];
		ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
		return dirName( std::string( result, (count > 0) ? count : 0 ) );
		#endif
	}

	///finds the folder with `data` and `locale` directories
	std::string getAssetRoot() {
		static bool found = false;
		static std::string assets;
		constexpr char* pathSep =
			#ifdef MSWIN_SYSTEM
			"\\"
			#else
			"/"
			#endif
			;

		if( !found ) {
			assets = getProgramDir();

			while( !pathExists( assets + pathSep + "data" ) || !pathExists( assets + pathSep + "locale" ) ) {
				assets += pathSep;
				assets += "..";
			}

			found = true;
		}

		return assets;
	}
	
	///makes a path for the given data name
	std::string getDataPath(std::string dataName) {
		#ifdef MSWIN_SYSTEM
		std::string dataSubDir = std::string("\\data\\");
		#else
		std::string dataSubDir = std::string("/data/");
		#endif
		return getAssetRoot() + dataSubDir + dataName;
	}
	
	///checks if path exists
	bool pathExists(std::string path) {
		#ifdef MSWIN_SYSTEM
		std::wstring W_path = toWString(path);
		struct _stat info;
		return ( _wstat(W_path.c_str(), &info) == 0 );
		#else
		struct stat info;
		return ( stat(path.c_str(), &info) == 0 );
		#endif
	}
	
	///gets path to the configuration file
	std::string getConfigurationFile(bool createIfNecessary) {
		std::string home;
		#ifdef MSWIN_SYSTEM
		wchar_t *path = nullptr;
		if( SHGetKnownFolderPath(FOLDERID_RoamingAppData,0,nullptr,&path) == S_OK ) {
			//got the path
			size_t wsize = wcslen(path);
			home = fromWString(path, wsize);
		} else {
			home = getProgramDir();
		}
		home += "\\BodyReSwitcher.conf";
		#else
		char* e = getenv("HOME");
		if( e == nullptr ) {//it CAN happen
			home = getProgramDir();
		} else {
			home = std::string(e);
		}
		home += "/.bodyreswitcher";
		#endif
		
		
		if( createIfNecessary && !pathExists(home) ) {
			FILE* f = fopen(home.c_str(), "w");
			fprintf(f,"\n");
			fclose(f);
		}
		
		return home;
	}
	
}//end namespace filesystem

