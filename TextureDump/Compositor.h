#pragma once
#include "materialsystem/itexturecompositor.h"

ITextureCompositor* CreateTextureCompositor(int paintKitDefIndex, int itemIndex, int wear, int teamNum, uint64 seed, TextureCompositeCreateFlags_t nCompositeFlags = TextureCompositeCreateFlags_t::TEX_COMPOSITE_CREATE_FLAGS_NO_COMPRESSION);

