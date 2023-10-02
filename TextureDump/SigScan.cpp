#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "SigScan.h"

#include <libloaderapi.h>
#include <Psapi.h>
#include <processthreadsapi.h>
#include "dbg.h"

void* FindModuleBytePattern(const char* moduleName, const char* searchBytes, size_t searchBytesLength)
{
	Msg("Searching for byte pattern...\n");
	auto moduleHandle = GetModuleHandle(moduleName);
	if (moduleHandle == 0)
	{
		Msg("moduleHandle is NULL. Aborting.\n");
		return NULL;
	}

	MODULEINFO moduleInfo;
	if (!GetModuleInformation(GetCurrentProcess(), moduleHandle, &moduleInfo, sizeof(moduleInfo)))
	{
		Msg("GetModuleInformation failed. Aborting.\n");
		return NULL;
	}

	const char* moduleStart = (const char*)moduleInfo.lpBaseOfDll;
	const char* moduleEnd = moduleStart + moduleInfo.SizeOfImage;

	const char* readEnd = moduleEnd - searchBytesLength;

	int highestMatchCount = 0;

	for (const char* readStartPos = moduleStart; readStartPos < readEnd; ++readStartPos)
	{
		bool matched = true;
		for (int i = 0; i < searchBytesLength; ++i)
		{
			if (readStartPos[i] != searchBytes[i])
			{
				matched = false;
				break;
			}
			highestMatchCount = Max(highestMatchCount, i);
		}
		if (matched)
		{
			Msg("Found match at %d!\n", (int)readStartPos);
			return (void*)readStartPos;
		}
	}

	Msg("Did not find pattern. Longest match: %d. Aborting.\n", highestMatchCount);
	return NULL;
}