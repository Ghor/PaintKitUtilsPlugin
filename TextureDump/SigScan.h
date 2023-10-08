#pragma once

#include "Pattern.h"
#include <memory>
#include <basetypes.h>
#include "ModuleHelper.h"

const void* FindModuleBytePattern(ModuleHelper* moduleHelper, Pattern* pattern);

template<typename T>
class SigScannedFunc
{
public:
	SigScannedFunc(const char* moduleName, std::unique_ptr<Pattern>&& pattern) :
		moduleHelper(moduleName)
	{
		this->pattern = std::move(pattern);
		this->func = NULL;
	}

	T GetFunc()
	{
		if (!func)
		{
			func = (T)FindModuleBytePattern(&moduleHelper, pattern.get());
		}
		return func;
	}

	bool IsValid() { return func != 0; }

private:
	ModuleHelper moduleHelper;
	T func;
	std::unique_ptr<Pattern> pattern;
};

template<typename T>
class OffsetFunc
{
public:
	OffsetFunc(const char* moduleName, int offset) :
		moduleHelper(moduleName)
	{
		this->offset = offset;
		this->func = NULL;
	}

	T GetFunc()
	{
		if (!func)
		{
			func = (T)moduleHelper.OffsetToPtr(offset);
		}
		return func;
	}

	bool IsValid() { return func != 0; }

private:
	T func;
	ModuleHelper moduleHelper;
	int offset;
};
