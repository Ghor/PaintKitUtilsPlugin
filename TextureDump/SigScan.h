#pragma once



void* FindModuleBytePattern(const char* moduleName, const char* searchBytes, size_t searchBytesLength);

template<typename T>
class SigScannedFunc
{
public:
	template<int N>
	SigScannedFunc(const char* moduleName, const char(&searchBytes)[N])
	{
		this->moduleName = moduleName;
		this->searchBytes = searchBytes;
		this->searchBytesLength = N;
	}

	T GetFunc()
	{
		if (!func)
		{
			func = (T)FindModuleBytePattern(moduleName, searchBytes, searchBytesLength);
		}
		return func;
	}

	bool IsValid() { return func != 0; }

private:
	T func;
	const char* moduleName;
	const char* searchBytes;
	int searchBytesLength;
};