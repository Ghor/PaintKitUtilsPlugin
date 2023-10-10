#include "CmdArgOneOf.h"

int CmdArgOneOfBase::FindUserArgIndex(CCommand args) const
{
	int userProvidedArgIndex = -1;
	for (int i = 0; i < optionNames.Count(); ++i)
	{
		const char* optionName = optionNames[i];
		const char* argValue = args.FindArg(optionName);
		if (argValue != NULL)
		{
			if (userProvidedArgIndex < 0)
			{
				userProvidedArgIndex = i;
			}
			else
			{
				ThrowTooManyException();
			}
		}
	}
	if (userProvidedArgIndex < 0)
	{
		ThrowNoneOfException();
	}
	return userProvidedArgIndex;
}

void CmdArgOneOfBase::ThrowTooManyException() const
{
	CUtlString msg;
	msg.Append("Cannot provide more than one of: ");
	for (int i = 0, end = optionNames.Count(), last = end - 1; i < end; ++i)
	{
		msg.Append("\"");
		msg.Append(optionNames[i]);
		msg.Append("\"");
		if (i < last)
		{
			msg.Append(" ");
		}
	}
	throw std::runtime_error(msg);
}

void CmdArgOneOfBase::ThrowNoneOfException() const
{
	CUtlString msg;
	msg.Append("Must provide one of: ");
	for (int i = 0, end = optionNames.Count(), last = end - 1; i < end; ++i)
	{
		msg.Append("\"");
		msg.Append(optionNames[i]);
		msg.Append(" X\"");
		if (i < last)
		{
			msg.Append(" ");
		}
	}
	throw std::runtime_error(msg);
}
