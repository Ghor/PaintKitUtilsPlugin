#include "FileUtils.h"
#include "fileio.h"

CUtlString GetFileNameFromPath(const char* path)
{
	const char* lastSlash = strrchr(path, '/');
	const char* lastBackslash = strrchr(path, '\\');

	const char* fileNameStart = Max(lastSlash, lastBackslash);

	if (fileNameStart != NULL)
	{
		++fileNameStart; // Advance over the slash.
	}
	else
	{
		fileNameStart = path; // Just use the whole string.
	}

	return CUtlString(fileNameStart);
}

CUtlString GetDirectoryFromPath(const char* path)
{
	const char* lastSlash = strrchr(path, '/');
	const char* lastBackslash = strrchr(path, '\\');

	const char* pathEnd = Max(lastSlash, lastBackslash);

	if (pathEnd != NULL)
	{
		return "";
	}

	return CUtlString(path, pathEnd - path);
}
