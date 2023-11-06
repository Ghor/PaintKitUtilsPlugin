#include "Pattern.h"
#include "basetypes.h"

bool BasicPattern::IsMatch(const byte* target, int targetLength, int* matchingByteCount)
{
	for (int i = 0, iEnd = Min((int)byteSeries.size(), targetLength); i < iEnd; ++i)
	{
		if (target[i] == byteSeries[i])
		{
			++(*matchingByteCount);
		}
		else
		{
			break;
		}
	}
	return (*matchingByteCount) == byteSeries.size();
}

bool WildcardPattern::IsMatch(const byte* target, int targetLength, int* matchingByteCount)
{
	*matchingByteCount = Min(numBytes, targetLength);
	return *matchingByteCount == numBytes;
}

CompoundPattern::CompoundPattern(const std::initializer_list<Pattern*>& children)
{
	this->childrenLength = children.size();
	this->children = new std::unique_ptr<Pattern>[this->childrenLength];
	int i = 0;
	for (auto it = children.begin(), it_end = children.end(); it != it_end; ++it)
	{
		this->children[i++].reset(*it);
	}
}

CompoundPattern::~CompoundPattern()
{
	if (children)
	{
		delete[] children;
		children = NULL;
	}
}

bool CompoundPattern::IsMatch(const byte* target, int targetLength, int* matchingByteCount)
{
	*matchingByteCount = 0;
	for (int i = 0; i < childrenLength; ++i)
	{
		int childMatchCount = 0;
		bool childSuccess = children[i]->IsMatch(target + *matchingByteCount, targetLength - *matchingByteCount, &childMatchCount);
		*matchingByteCount += childMatchCount;
		if (!childSuccess)
		{
			return false;
		}
	}
	return true;
}
