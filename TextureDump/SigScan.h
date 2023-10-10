#pragma once

#include "Pattern.h"
#include <memory>
#include <basetypes.h>
#include "ModuleHelper.h"

#include "utlstring.h"

const void* FindModuleBytePattern(ModuleHelper* moduleHelper, Pattern* pattern);

template<typename T>
class SigScannedFunc
{
public:
	SigScannedFunc(const char* moduleName, const char* funcName, std::unique_ptr<Pattern>&& pattern) :
		func(NULL),
		moduleHelper(moduleName),
		pattern(std::move(pattern)),
		funcName(funcName)
	{
	}

	T GetFunc()
	{
		if (!func)
		{
			func = (T)FindModuleBytePattern(&moduleHelper, pattern.get());
			if (!func)
			{
				CUtlString msg;
				msg.Format("Failed to find func \"%s\"!\n", funcName);
				throw std::runtime_error(msg.Get());
			}
		}
		return func;
	}

	bool IsValid() { return func != 0; }

private:
	T func;
	ModuleHelper moduleHelper;
	std::unique_ptr<Pattern> pattern;
	const char* funcName;
};

template<typename T>
class OffsetFunc
{
public:
	OffsetFunc(const char* moduleName, const char* funcName, int offset) :
		func(NULL),
		moduleHelper(moduleName),
		offset(offset),
		funcName(funcName)
	{
	}

	T GetFunc()
	{
		if (!func)
		{
			func = (T)moduleHelper.OffsetToPtr(offset);
			if (!func)
			{
				CUtlString msg;
				msg.Format("Failed to find func \"%s\"!\n", funcName);
				throw std::runtime_error(msg.Get());
			}
		}
		return func;
	}

	bool IsValid() { return func != 0; }

private:
	T func;
	ModuleHelper moduleHelper;
	int offset;
	const char* funcName;
};
