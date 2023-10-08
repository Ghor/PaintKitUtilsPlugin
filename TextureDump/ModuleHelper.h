#pragma once
#include <stdexcept>

class ModuleHelper
{
public:
	class ModuleInfo;
	typedef unsigned char byte;

public:
	ModuleHelper(const char* moduleName);

	~ModuleHelper();

	bool IsResolved();

	void GetSpan(const byte** moduleStart, int* moduleSize);
	const void* OffsetToPtr(int offset);
	int PtrToOffset(const void* ptr);

	class ModuleNotLoadedException : public std::runtime_error
	{
	public:
		ModuleNotLoadedException(const char* moduleName);
		virtual ~ModuleNotLoadedException() override;
		virtual const char* what() const override;
	private:
		const char* moduleName;
		mutable char* msg;
	};

private:
	const char* moduleName;
	ModuleInfo* moduleInfo;
};
