#pragma once

#include "IProtoBufScriptObjectDefinition.h"
#include "tier1/utldict.h"

template<typename TMsg, int typeID>
class CTypedProtoBufScriptObjectDefinition : public IProtoBufScriptObjectDefinition
{
public:
	virtual ~CTypedProtoBufScriptObjectDefinition() = 0;
	virtual TMsg* GetMessage() = 0;
	virtual void vfunction3() = 0;
	virtual const char* GetDescriptionToken() = 0;
	virtual void vfunction5() = 0;
	virtual void vfunction6() = 0;
	virtual void vfunction7() = 0;
	virtual void vfunction8() = 0;
	virtual void vfunction9() = 0;

	TMsg m_msg1;
	TMsg m_msg2;
	CUtlDict<KeyValues*, unsigned short> m_dict;

	char gap[6 * 4];
};
