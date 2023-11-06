#include "ModuleHelper.h"

#include <stdexcept>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // Prerequisite for Windows headers to work.
#include <Psapi.h> // GetModuleInformation
#include <libloaderapi.h> // For GetModuleHandle
#include <processthreadsapi.h> // For GetCurrentProcess

#define MODULE_NAME_COMPARER stricmp // Case-insensitive on Windows.
#define MODULE_NAME_CASE_INSENSITIVE 1
#endif

#include "utlvector.h"

namespace {
	inline const char* CloneCString(const char* src, int length)
	{
		char* dest = new char[length + 1];
		strncpy(dest, src, length + 1);
		return dest;
	}
}

class ModuleHelper::ModuleInfo
{
#ifdef WIN32
public:
	typedef MODULEINFO impl_t;
#else
	static_assert(0, "ModuleHelper::impl_t not yet defined for this platform.");
#endif

	ModuleInfo(const char* moduleName, int moduleNameLength) :
		moduleName(CloneCString(moduleName, moduleNameLength)),
		impl(),
		isResolved(false)
	{
	}

	~ModuleInfo()
	{
		delete[] moduleName;
	}

	inline impl_t& GetImpl()
	{
		return impl;
	}

	inline bool IsResolved() const { return isResolved; }

	inline const char* const& GetModuleName() const { return moduleName; }

	bool __declspec(noinline) Resolve()
	{
		if (isResolved)
		{
			return true;
		}
#ifdef WIN32
		auto moduleHandle = GetModuleHandle(moduleName);
		if (!moduleHandle)
		{
			return false;
		}

		if (!GetModuleInformation(GetCurrentProcess(), moduleHandle, &impl, sizeof(impl)))
		{
			// Failed. Zero the state.
			impl = impl_t();
			return false;
		}
#else
		static_assert(0, __FUNCTION__ " not yet implemented for this platform.");
#endif
		isResolved = true;
		return true;
	}

private:
	impl_t impl;
	bool isResolved;
	const char* moduleName;
};


namespace {
	class ModuleCatalog
	{
	public:
		~ModuleCatalog()
		{
			for (int i = 0; i < moduleInfos.Size(); ++i)
			{
				delete moduleInfos[i];
			}
		}

		ModuleHelper::ModuleInfo* GetOrCreateModuleInfo(const char* moduleName)
		{
			int moduleNameLength = strlen(moduleName);

			for (int i = 0; i < moduleInfos.Size(); ++i)
			{
				ModuleHelper::ModuleInfo* current = moduleInfos[i];
				const char* currentModuleName = current->GetModuleName();
#if MODULE_NAME_CASE_INSENSITIVE
				if (strnicmp(moduleName, currentModuleName, moduleNameLength) == 0)
#else
				if (strncmp(moduleName, currentModuleName, moduleNameLength) == 0)
#endif
				{
					return moduleInfos[i];
				}
			}
			ModuleHelper::ModuleInfo* result = new ModuleHelper::ModuleInfo(moduleName, moduleNameLength);
			moduleInfos.AddToTail(result);
			return result;
		}

	private:
		CUtlVector<ModuleHelper::ModuleInfo*> moduleInfos;
	};

	ModuleCatalog moduleCatalog;
}

ModuleHelper::ModuleHelper(const char* moduleName) :
	moduleName(moduleName),
	moduleInfo(moduleCatalog.GetOrCreateModuleInfo(moduleName))
{
}

ModuleHelper::~ModuleHelper()
{
}

bool ModuleHelper::IsResolved()
{
	return moduleInfo->Resolve();
}

void ModuleHelper::GetSpan(const byte** moduleStart, int* moduleSize)
{
	if (!moduleInfo->Resolve())
	{
		throw ModuleHelper::ModuleNotLoadedException(moduleName);
	}
#ifdef WIN32
	if (moduleStart)
	{
		*moduleStart = (byte*)moduleInfo->GetImpl().lpBaseOfDll;
	}
	if (moduleSize)
	{
		*moduleSize = moduleInfo->GetImpl().SizeOfImage;
	}
#else
	static_assert(0, __FUNCTION__ " not yet implemented for this platform.");
#endif
	return;
}

const void* ModuleHelper::OffsetToPtr(int offset)
{
	if (!moduleInfo->Resolve())
	{
		throw ModuleHelper::ModuleNotLoadedException(moduleName);
	}

	const byte* moduleStart = NULL;
	GetSpan(&moduleStart, NULL);

	return (const void*)(moduleStart + offset);
}

int ModuleHelper::PtrToOffset(const void* ptr)
{
	if (!moduleInfo->Resolve())
	{
		throw ModuleHelper::ModuleNotLoadedException(moduleName);
	}

	const byte* moduleStart = NULL;
	GetSpan(&moduleStart, NULL);

	return ((byte*)ptr) - moduleStart;
}

ModuleHelper::ModuleNotLoadedException::ModuleNotLoadedException(const char* moduleName) :
	runtime_error(NULL),
	moduleName(moduleName),
	msg(NULL)
{
}

ModuleHelper::ModuleNotLoadedException::~ModuleNotLoadedException()
{
	delete[] msg;
}

const char* ModuleHelper::ModuleNotLoadedException::what() const
{
	if (msg == NULL) // msg is only built on-demand.
	{
		const char* format = "Attempt to access unresolved module \"%s\".";
		const int formatLength = strlen(format);
		const int moduleNameLength = strlen(moduleName);
		const int msgLength = formatLength - 2 + moduleNameLength + sizeof('\0');
		msg = new char[msgLength];
		snprintf(msg, msgLength, format, moduleName);
	}
	return msg;
}
