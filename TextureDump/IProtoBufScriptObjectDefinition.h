#pragma once
#include "tf_proto_def_messages.pb.h"

class IProtoBufScriptObjectDefinition
{
public:
	virtual ~IProtoBufScriptObjectDefinition() = 0;
	virtual int vfunction2() = 0;
	virtual ProtoDefTypes GetTypeID() const = 0;
	virtual const char* GetDescriptionToken() = 0;
	virtual void vfunction5() = 0;
	virtual void vfunction6() = 0;
	virtual void vfunction7() = 0;
	virtual void vfunction8() = 0;
	virtual google::protobuf::Message* vfunction9() const = 0;
private:
	char unknownData[0x20 - sizeof(void*) /*vftable*/];
};
static_assert(sizeof(IProtoBufScriptObjectDefinition) == 0x20, "sizeof(IProtoBufScriptObjectDefinition) must be 0x20.");
