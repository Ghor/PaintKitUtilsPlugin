#pragma once

class IProtoBufScriptObjectDefinition
{
	char unknownData[0x20];
};

static_assert(sizeof(IProtoBufScriptObjectDefinition) == 0x20, "sizeof(IProtoBufScriptObjectDefinition) must be 0x20.");
