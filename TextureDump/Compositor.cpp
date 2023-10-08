
#include "Compositor.h"

#include "materialsystem/imaterialsystem.h"
#include "vstdlib/IKeyValuesSystem.h"
#include "KeyValues.h"
#include "CPaintKitDefinition.h"
#include "CPaintKitItemDefinition.h"
#undef GetMessage

ITextureCompositor* CreateTextureCompositor(const CPaintKitDefinition* paintKitDef, const CPaintKitItemDefinition* itemDef, int wear, int teamNum, uint64 seed, TextureCompositeCreateFlags_t nCompositeFlags, int width, int height)
{
	KeyValues* stageDesc = paintKitDef->GenerateStageDescKeyValues(itemDef->m_msg2.item_definition_index(), wear);

	char finalItemName[_MAX_PATH];
	V_sprintf_safe(finalItemName, "%d_%d_wear_%02d", itemDef->m_msg2.item_definition_index(), paintKitDef->m_msg2.header().defindex(), wear); // Same format used by the game.
	
	ITextureCompositor* result = materials->NewTextureCompositor(width, height, finalItemName, teamNum, seed, stageDesc, nCompositeFlags);
	if (result == NULL)
	{
		Warning("IMaterialSystem::NewTextureCompositor(%d, %d, %s, %d, %ull, 0x%X, %d) failed!", width, height, finalItemName, teamNum, seed, (int)stageDesc, nCompositeFlags);
	}
	return result;
}
