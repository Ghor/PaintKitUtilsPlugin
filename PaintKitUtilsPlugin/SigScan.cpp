#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "SigScan.h"
#include "dbg.h"

const void* FindModuleBytePattern(ModuleHelper* moduleHelper, Pattern* pattern)
{
	// Msg("Searching for byte pattern...\n");

	const byte* moduleStart = NULL;
	int moduleSize = 0;

	try {
		moduleHelper->GetSpan(&moduleStart, &moduleSize);

		const byte* moduleEnd = moduleStart + moduleSize;

		int highestMatchCount = 0;

		for (const byte* currentReadPos = moduleStart; currentReadPos < moduleEnd; ++currentReadPos)
		{
			bool matched = true;

			int bytesRead = 0;
			if (pattern->IsMatch(currentReadPos, moduleSize, &bytesRead))
			{
				// Msg("Found match at 0x%X! bytesRead=%d offset=0x%X\n ", (int)currentReadPos, bytesRead, currentReadPos - moduleStart);
				return (void*)currentReadPos;
			}

			highestMatchCount = Max(highestMatchCount, bytesRead);
		}
	}
	catch (ModuleHelper::ModuleNotLoadedException e)
	{
		Warning("FindModuleBytePattern failed: ", e.what());
	}

	// Msg("Did not find pattern. Longest match: %d. Aborting.\n", highestMatchCount);
	return NULL;
}
