#include "filesystem.h"

#ifdef _WIN32

#include <Windows.h>

bool POCNES::dirExists(const char* path) {
	DWORD ftyp = GetFileAttributesA(path);

	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;	// Not a valid path

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;	// Path exists

	return false;		// Path is not a directory
}

bool POCNES::makedir(const char* path) {
	return CreateDirectoryA(path, NULL);
}

#elif __linux__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


bool POCNES::dirExists(const char* path) {
	struct stat sb;

	if (stat(path, &sb) == -1)
		return false;

	return S_ISDIR(sb.st_mode);
}

bool POCNES::makedir(const char* path) {
	// TOOD: Implement this for Linux

	return false;
}

#else

bool POCNES::dirExists(const char* path) {
	return false;
}

bool POCNES::makedir(const char* path) {
	return false;
}

#endif


