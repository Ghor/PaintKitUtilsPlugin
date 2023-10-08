#pragma once
#include "materialsystem/itexturecompositor.h"

class CPaintKitDefinition;
class CPaintKitItemDefinition;

ITextureCompositor* CreateTextureCompositor(const CPaintKitDefinition* paintKitDef, const CPaintKitItemDefinition* itemDef, int wear, int teamNum, uint64 seed, TextureCompositeCreateFlags_t nCompositeFlags = TextureCompositeCreateFlags_t::TEX_COMPOSITE_CREATE_FLAGS_NO_COMPRESSION, int width = 2048, int height = 2048);
