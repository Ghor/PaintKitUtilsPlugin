#include "TextureUtils.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/itexture.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "KeyValues.h"
#include "pixelwriter.h"
#include "bitmap/tgawriter.h"

#include "FileUtils.h"
#include "filesystem.h"

const char* texture_group_names[] = {
TEXTURE_GROUP_LIGHTMAP,
TEXTURE_GROUP_WORLD,
TEXTURE_GROUP_MODEL,
TEXTURE_GROUP_VGUI,
TEXTURE_GROUP_PARTICLE,
TEXTURE_GROUP_DECAL,
TEXTURE_GROUP_SKYBOX,
TEXTURE_GROUP_CLIENT_EFFECTS,
TEXTURE_GROUP_OTHER,
TEXTURE_GROUP_PRECACHED,
TEXTURE_GROUP_CUBE_MAP,
TEXTURE_GROUP_RENDER_TARGET,
TEXTURE_GROUP_RUNTIME_COMPOSITE,
TEXTURE_GROUP_UNACCOUNTED,
TEXTURE_GROUP_STATIC_INDEX_BUFFER,
TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP,
TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR,
TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD,
TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS,
TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER,
TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER,
TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER,
TEXTURE_GROUP_DEPTH_BUFFER,
TEXTURE_GROUP_VIEW_MODEL,
TEXTURE_GROUP_PIXEL_SHADERS,
TEXTURE_GROUP_VERTEX_SHADERS,
TEXTURE_GROUP_RENDER_TARGET_SURFACE,
TEXTURE_GROUP_MORPH_TARGETS,
};

namespace {
	template<typename T>
	class SafePtr
	{
	public:
		SafePtr(T* other)
		{
			SafeAssign(&ptr, other);
		}

		~SafePtr()
		{
			SafeRelease(&ptr);
		}


		SafePtr& operator=(T* other) { SafeAssign(&ptr, other); return this; }
		SafePtr& operator=(SafePtr& other); // Don't allow copying, we have ownership.
		SafePtr& operator=(SafePtr&& other) { std::swap(this, other); return this; } // Move is fine.

		operator T* () const { return ptr; }

		T* operator->() const { return ptr; }

	private:
		T* ptr;
	};

	template<typename T, int N>
	constexpr int StaticSize(T arr[N])
	{
		return N;
	}
}

class TempPixelBuffer
{
public:
	TempPixelBuffer(unsigned int width, unsigned int height, size_t pixel_size) :
		pixel_size(pixel_size),
		buffer_length(width* height* pixel_size)
	{
		buffer = new unsigned char[buffer_length];
	}

	~TempPixelBuffer()
	{
		if (buffer != NULL)
		{
			delete[] buffer;
			buffer = NULL;
		}
	}

	operator unsigned char* () const { return buffer; }
	unsigned char* operator->() const { return buffer; }
	size_t GetPixelSize() const { return pixel_size; }
	size_t GetBufferLength() const { return buffer_length; }

	unsigned char* GetBuffer() { return buffer; }

	bool CheckForGoodValues() const
	{
		for (int i = 0; i < buffer_length; ++i)
		{
			unsigned char value = buffer[i];
			if ((value != 0) && (value != 0xFF))
			{
				return true;
			}
		}
		return false;
	}

private:
	unsigned char* buffer;
	size_t buffer_length;
	size_t pixel_size;
};

bool SaveTextureToDisk(ITexture* texture, const char* outputFilePath)
{
	int width = texture->GetActualWidth();
	int height = texture->GetActualHeight();
	Rect_t src_rect = { 0, 0, width, height };
	Rect_t dest_rect = src_rect;

	SafePtr<ITexture> textureDumpRT = materials->CreateNamedRenderTargetTextureEx("texture_dump_rt", width, height, RT_SIZE_LITERAL, IMAGE_FORMAT_BGRA8888, MATERIAL_RT_DEPTH_NONE, TEXTUREFLAGS_RENDERTARGET | TEXTUREFLAGS_EIGHTBITALPHA | TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_POINTSAMPLE | TEXTUREFLAGS_NOLOD);

	if ((textureDumpRT == NULL) || (textureDumpRT->IsError()))
	{
		Warning("Could not allocate render target.\n");
		return false;
	}

	KeyValues * pVMTKeyValues = new KeyValues("UnlitGeneric");
	pVMTKeyValues->SetString("$basetexture", "");
	SafePtr<IMaterial> textureDumpMat = materials->CreateMaterial("texture_dump_mat", pVMTKeyValues);

	if (textureDumpMat == NULL)
	{
		Warning("Could not generate material.");
		return false;
	}

	if (textureDumpMat != NULL)
	{
		bool bFoundVar;
		IMaterialVar* basetextureVar = textureDumpMat->FindVar("$basetexture", &bFoundVar);
		if (bFoundVar)
		{
			basetextureVar->SetTextureValue(texture);
		}
		else
		{
			Warning("Could not find $basetexture var to assign.");
			return false;
		}

	}

	// Is this actually neccessary? Will have to check, since we've created new materials/textures on this frame.
	materials->Flush();

	TempPixelBuffer pixels = TempPixelBuffer(width, height, sizeof(BGRA8888_t));

	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->BeginRender();
	pRenderContext->ClearColor4ub(0, 0, 0xFF, 0xFF);
	pRenderContext->ClearBuffers(true, true, true);
	pRenderContext->PushRenderTargetAndViewport(textureDumpRT, 0, 0, width, height);

	pRenderContext->DrawScreenSpaceRectangle(textureDumpMat, 0, 0, width, height, 0.0, 0.0, width - 1, height - 1, width, height);
	pRenderContext->ReadPixels(0, 0, width, height, pixels.GetBuffer(), IMAGE_FORMAT_BGRA8888);

	pRenderContext->PopRenderTargetAndViewport();
	pRenderContext->EndRender();

	// Ensure necessary directories exist.
	g_pFullFileSystem->CreateDirHierarchy(GetDirectoryFromPath(outputFilePath));

	bool success = TGAWriter::WriteTGAFile(outputFilePath, width, height, textureDumpRT->GetImageFormat(), pixels, width * pixels.GetPixelSize());

	if (!success) {
		Warning("Failed to write texture to file \"%s\".\n", outputFilePath);
		return false;
	}

	Msg("Successfully wrote texture to file \"%s\".\n", outputFilePath);
	return true;
}

void FindTextureAllGroups(const char* texture_name, int* num_found, ITexture** first_found)
{
	*num_found = 0;
	*first_found = NULL;
	for (int i = 0; i < std::size(texture_group_names); ++i)
	{
		const char* texture_group_name = texture_group_names[i];
		ITexture* texture = materials->FindTexture(texture_name, texture_group_name);
		if ((texture != NULL) && (!texture->IsError()))
		{
			++(*num_found);
			if (*first_found == NULL)
			{
				*first_found = texture;
			}
		}
	}
}