#pragma once

#include "CTypedProtoBufScriptObjectDefinition.h"
#include "tf_proto_def_messages.pb.h"

class KeyValues;

class CPaintKitDefinition : public CTypedProtoBufScriptObjectDefinition<CMsgPaintKit_Definition, ProtoDefTypes::DEF_TYPE_PAINTKIT_DEFINITION>
{
public:
	KeyValues* GenerateStageDescKeyValues(unsigned int itemDefinitionIndex, int wear) const;
};

