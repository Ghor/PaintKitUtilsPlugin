#pragma once
#include "CTypedProtoBufScriptObjectDefinition.h"
#include "tf_proto_def_messages.pb.h"

// Size is 0x7C
class CPaintKitItemDefinition : public CTypedProtoBufScriptObjectDefinition<CMsgPaintKit_ItemDefinition, ProtoDefTypes::DEF_TYPE_PAINTKIT_ITEM_DEFINITION>
{
};
