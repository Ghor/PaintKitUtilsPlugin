#pragma once
#include "utlvector.h"
#include "utlstring.h"
#include "convar.h"
#include <initializer_list>
#include <stdexcept>

class CmdArgOneOfBase
{
protected:
	int FindUserArgIndex(CCommand args) const;

private:
	void ThrowTooManyException() const;

	void ThrowNoneOfException() const;

protected:
	CUtlVector<const char*> optionNames;
};

template<typename RESULT_T>
class CmdArgOneOf : public CmdArgOneOfBase
{
public:
	using Result_t = RESULT_T;

	using ResolverImpl_t = bool(const char* inputStr, Result_t* result);

	class Resolver
	{
	public:
		virtual ~Resolver() = default;
		virtual bool Resolve(const char* inputStr, Result_t* result) = 0;
	};

	class BasicResolver : Resolver
	{
	public:
		BasicResolver(ResolverImpl_t* impl) : impl(impl) {};
		virtual bool Resolve(const char* inputStr, Result_t* result) override { return impl(inputStr, result); }
	private:
		ResolverImpl_t* impl;
	};

	CmdArgOneOf(const std::initializer_list<std::pair<const char*, ResolverImpl_t*>>& l)
	{
		for (auto it = l.begin(), it_end = l.end(); it != it_end; ++it)
		{
			auto pair = (*it);
			optionNames.AddToHead(pair.first);
			optionResolvers.AddToHead(pair.second);
		}
	}

public:
	bool Resolve(CCommand args, Result_t* result, const char** usedArgName, const char** usedArgValue)
	{
		int userArgIndex = FindUserArgIndex(args);
		if (userArgIndex < 0)
		{
			return false;
		}

		const char* argName = optionNames[userArgIndex];
		const char* argValue = args.FindArg(argName);
		if (usedArgName)
		{
			*usedArgName = argName;
		}
		if (usedArgValue)
		{
			*usedArgValue = argValue;
		}
		
		ResolverImpl_t* resolver = optionResolvers[userArgIndex];
		return resolver(argValue, result);
	}

private:
	CUtlVector<ResolverImpl_t*> optionResolvers;
};
