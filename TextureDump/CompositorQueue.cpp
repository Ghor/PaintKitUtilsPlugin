#include "CompositorQueue.h"
#include "materialsystem/itexturecompositor.h"
#include "Compositor.h"
#include "TextureUtils.h"
#include "refcount.h"
#include "UtlString.h"
#include "fileio.h"
#include "CProtoBufScriptObjectDefinitionManager.h"
#include <exception>
#include <stdexcept>
#include "FileUtils.h"


CompositorQueue::~CompositorQueue()
{
	while (!operationQueue.IsEmpty())
	{
		delete operationQueue.RemoveAtHead();
	}
}

void CompositorQueue::Update()
{
	if (operationQueue.IsEmpty())
	{
		return;
	}
	auto currentOperation = operationQueue.Head();

	try
	{
		if (currentOperation->GetTextureCompositor() == NULL)
		{
			// The operation currently has no compositor, so let's get it started.
			CompositorQueue::CompositorRequest& request = currentOperation->request;


			currentOperation->SetTextureCompositor(CreateTextureCompositor(
				request.paintKitDef,
				request.itemDef,
				request.wear,
				request.team,
				request.seed,
				TextureCompositeCreateFlags_t::TEX_COMPOSITE_CREATE_FLAGS_NO_COMPRESSION,
				request.width,
				request.height
			));

			// If we failed to get a new compositor, bail on this operation.
			if (currentOperation->GetTextureCompositor() == NULL)
			{
				throw std::runtime_error("Failed to create texture compositor.");
			}

			// If all went well, schedule the compositor to resolve.
			currentOperation->GetTextureCompositor()->ScheduleResolve();
		}

		// Check in on the status of our current operation.
		switch (currentOperation->GetTextureCompositor()->GetResolveStatus())
		{
		case ECompositeResolveStatus::ECRS_Idle:
		{
			// Shouldn't be idle...
		} break;
		case ECompositeResolveStatus::ECRS_Scheduled:
		{
		} break;
		case ECompositeResolveStatus::ECRS_PendingTextureLoads:
		{
		} break;
		case ECompositeResolveStatus::ECRS_PendingComposites:
		{
		} break;
		case ECompositeResolveStatus::ECRS_Error:
		{
			CompositorQueue::CompositorRequest& request = currentOperation->request;
			throw std::runtime_error("Error resolving texture compositor.\n");
		} break;
		case ECompositeResolveStatus::ECRS_Complete:
		{
			ITexture* resultTexture = currentOperation->GetTextureCompositor()->GetResultTexture();
			
			CUtlString outputDir = currentOperation->request.outputDir;

			// Default to "dump" if no output directory was explicitly specified.
			if (outputDir.IsEmpty())
			{
				outputDir = "dump";
			}

			CUtlString fileName = currentOperation->request.outputName;

			// Default to the engine's name for the texture if the request doesn't contain a name override.
			if (fileName.IsEmpty())
			{
				fileName = GetFileNameFromPath(resultTexture->GetName());
			}

			CUtlString outputPath = currentOperation->request.outputDir + "/" + fileName + ".tga";

			SaveTextureToDisk(resultTexture, outputPath);

			delete operationQueue.RemoveAtHead();
			resultTexture->Release();
		} break;
		}
	}
	catch (std::exception e)
	{
		Warning("Encountered error in compositor operation for request %s, discarding. Error: %s\n", currentOperation->request.ToDebugString().Get(), e.what());
		delete operationQueue.RemoveAtHead();
	}

}

void CompositorQueue::EnqueueRequest(const CompositorRequest& request)
{
	CompositorOperation* operation = new CompositorOperation();
	operation->request = request;
	operationQueue.Insert(operation);
}

CompositorQueue* CompositorQueue::Singleton()
{
	static CompositorQueue singleton;
	return &singleton;
}

CompositorQueue::CompositorOperation::~CompositorOperation()
{
	SafeRelease(compositor);
}

void CompositorQueue::CompositorOperation::SetTextureCompositor(ITextureCompositor* newCompositor)
{
	SafeAssign(&compositor, newCompositor);
}

CUtlString CompositorQueue::CompositorRequest::ToDebugString()
{
	CUtlString paintKitIdentifier;
	if (paintKitDefIndex > 0)
	{
		paintKitIdentifier.Format("paintKitDefIndex=%d", paintKitDefIndex);
	}
	if (!paintKitDefName.IsEmpty())
	{
		paintKitIdentifier.Format("paintKitDefName=%s", paintKitDefName.Get());
	}

	CUtlString itemIdentifier;
	if (itemDefIndex > 0)
	{
		paintKitIdentifier.Format("itemDefIndex=%d", itemDefIndex);
	}
	if (!itemDefName.IsEmpty())
	{
		itemIdentifier.Format("itemDefName=%s", itemDefName.Get());
	}


	CUtlString str;
	str.Format("{ %s %s wear=%d team=%d seed=%lld width=%d height=%d }", paintKitIdentifier.Get(), itemIdentifier.Get(), wear, team, seed, width, height);
	return str;
}
