#pragma once
#include <initializer_list>
#include <memory>

class ByteSeries
{
public:
	typedef unsigned char byte;

	template<int N>
	inline ByteSeries(const byte (&bytes)[N]) :
		bytes(bytes),
		bytesLength(N)
	{
	}

	template<int N>
	inline ByteSeries(const char (&bytes)[N]) :
		bytes((const byte*)bytes),
		bytesLength(N - 1)
	{
	}

	inline const byte& operator[](int i) { return bytes[i]; }

	inline size_t size() { return bytesLength; }

private:
	const byte* bytes;
	size_t bytesLength;
};


class Pattern
{
public:
	typedef unsigned char byte;

	virtual ~Pattern() {}
	virtual bool IsMatch(const byte* target, int targetLength, int* matchingByteCount) = 0;
};

class BasicPattern : public Pattern
{
public:
	BasicPattern(ByteSeries byteSeries)
		: byteSeries(byteSeries)
	{
	}

	virtual bool IsMatch(const byte* target, int targetLength, int* matchingByteCount) override;

private:
	ByteSeries byteSeries;
};

class WildcardPattern : public Pattern
{
public:
	WildcardPattern(int numBytes) :
		numBytes(numBytes)
	{
	}

	virtual bool IsMatch(const byte* target, int targetLength, int* matchingByteCount) override;

private:
	int numBytes;
};

class CompoundPattern : public Pattern
{
public:
	CompoundPattern(const std::initializer_list<Pattern*>& children);

	~CompoundPattern();

	virtual bool IsMatch(const byte* target, int targetLength, int* matchingByteCount) override;

private:
	std::unique_ptr<Pattern>* children;
	int childrenLength;
};