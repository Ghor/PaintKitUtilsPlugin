#pragma once

class ITexture;

bool SaveTextureToDisk(ITexture* texture, const char* path);
void FindTextureAllGroups(const char* texture_name, int* num_found, ITexture** first_found);

