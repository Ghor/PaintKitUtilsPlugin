#pragma once

#include "IProtoBufScriptObjectDefinition.h"
#include "tier1/utldict.h"
#include "tf_proto_def_messages.pb.h"

class KeyValues;

template<typename TMsg, ProtoDefTypes typeID>
class CTypedProtoBufScriptObjectDefinition : public IProtoBufScriptObjectDefinition
{
public:
	TMsg* GetMessage() const { return (TMsg*)vfunction9(); }

public:
	TMsg m_msg1;
	TMsg m_msg2;
};
