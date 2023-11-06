#pragma once

#include "IProtoBufScriptObjectDefinition.h"
#include "tier1/utldict.h"
#include "tf_proto_def_messages.pb.h"

class KeyValues;

template<typename TMsg, ProtoDefTypes typeID>
class CTypedProtoBufScriptObjectDefinition : public IProtoBufScriptObjectDefinition
{
public:
	using TMsg_t = TMsg;

public:
	TMsg_t* GetMessage() const { return (TMsg_t*)vfunction9(); }

public:
	TMsg_t m_msg1;
	TMsg_t m_msg2;
};
