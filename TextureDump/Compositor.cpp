
#include "Compositor.h"

#include "materialsystem/imaterialsystem.h"
#include "vstdlib/IKeyValuesSystem.h"
#include "CPaintKitDefinition.h"
#include "IProtoBufScriptObjectDefinition.h"

#include "SigScan.h"



SigScannedFunc<KeyValues* (__thiscall*)(CPaintKitDefinition* paintKit, unsigned short itemIndex, int wear)> GetPaintKitKeyValues("client.dll", "\x55\x8B\xEC\x83\xEC\x28\x66\x8B\x45\x08\x53\x8B\x5D\x0C\x56\x57\x8B\xF9\x66\x89\x45\xF8\x66\x89\x45\xEC\x8D\x45\xEC\x50\x89\x5D\xF0\x8D\x8F\xF4\x01\x00\x00");
SigScannedFunc<IProtoBufScriptObjectDefinition* (__cdecl*)(CMsgProtoDefID protoDefID)> GetScriptObjectDefinition("client.dll", "\x55\x8B\xEC\x83\xEC\x20\x8B\x45\x08\x8D\x4D\xE0\x56\x57\x8B\x78\x10\x8B\x70\x14\xE8");


ITextureCompositor* CreateTextureCompositor(int paintKitDefIndex, int itemIndex, int wear, int teamNum, uint64 seed, TextureCompositeCreateFlags_t nCompositeFlags)
{
	void* ptr = GetScriptObjectDefinition.GetFunc();
	Msg("GetScriptObjectDefinition=%x\n", ptr);
	return NULL;

	// int paintKitDefIndex = 290; // Simple Spirits
	// unsigned short itemIndex = 200; // Scattergun

	const int textureWidth = 2048;
	const int textureHeight = 2048;

	char finalItemName[_MAX_PATH];

	CMsgProtoDefID paintKitID = CMsgProtoDefID();
	paintKitID.set_type(ProtoDefTypes::DEF_TYPE_PAINTKIT_DEFINITION);
	paintKitID.set_defindex(paintKitDefIndex);
	CPaintKitDefinition* paintKit = static_cast<CPaintKitDefinition*>(GetScriptObjectDefinition.GetFunc()(paintKitID));
	KeyValues* stageDesc = GetPaintKitKeyValues.GetFunc()(paintKit, itemIndex, wear);

	V_sprintf_safe(finalItemName, "%d_%d_wear_%02d", itemIndex, paintKitDefIndex, wear);

	return materials->NewTextureCompositor(textureWidth, textureHeight, finalItemName, teamNum, seed, stageDesc, nCompositeFlags);
}

