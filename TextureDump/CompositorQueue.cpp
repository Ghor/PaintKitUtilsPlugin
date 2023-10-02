#include "CompositorQueue.h"
#include "materialsystem/itexturecompositor.h"
#include "Compositor.h"
#include "TextureUtils.h"
#include "refcount.h"
#include "UtlString.h"

CompositorQueue::~CompositorQueue()
{
	while (!operationQueue.IsEmpty())
	{
		delete operationQueue.RemoveAtHead();
	}
}

CUtlString GetFileNameFromPath(const char* path)
{
	const char* lastSlash = strrchr(path, '/');
	const char* lastBackslash = strrchr(path, '\\');

	const char* fileNameStart = Max(lastSlash, lastBackslash);

	if (fileNameStart != NULL)
	{
		++fileNameStart; // Advance over the slash.
	}
	else
	{
		fileNameStart = path; // Just use the whole string.
	}

	return CUtlString(fileNameStart);
}

void CompositorQueue::Update()
{
	if (operationQueue.IsEmpty())
	{
		return;
	}
	auto currentOperation = operationQueue.Head();

	if (currentOperation->GetTextureCompositor() == NULL)
	{
		// The operation currently has no compositor, so let's get it started.
		CompositorQueue::CompositorRequest& request = currentOperation->request;
		currentOperation->SetTextureCompositor(CreateTextureCompositor(
			request.paintKitDefIndex,
			request.itemDefIndex,
			request.wear,
			request.team,
			request.seed
		));

		// If we failed to get a new compositor, bail on this operation.
		if (currentOperation->GetTextureCompositor() == NULL)
		{
			Warning("Could not create texture compositor for paintKitDefIndex=%d itemDefIndex=%d wear=%d team=%d seed=%lld. Cancelling operation.", request.paintKitDefIndex, request.itemDefIndex, request.wear, request.team, request.seed);
			delete operationQueue.RemoveAtHead();
			return;
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
			Warning("Error resolving texture compositor for paintKitDefIndex=%d itemDefIndex=%d wear=%d team=%d seed=%lld. Cancelling operation.", request.paintKitDefIndex, request.itemDefIndex, request.wear, request.team, request.seed);
			delete operationQueue.RemoveAtHead();
		} break;
		case ECompositeResolveStatus::ECRS_Complete:
		{
			CUtlString outputPath = GetFileNameFromPath(currentOperation->GetTextureCompositor()->GetResultTexture()->GetName()) + ".tga";
			SaveTextureToDisk(currentOperation->GetTextureCompositor()->GetResultTexture(), outputPath);
			delete operationQueue.RemoveAtHead();
		} break;
	}
}

void CompositorQueue::EnqueueRequest(const CompositorRequest& request)
{
	CompositorOperation* operation = new CompositorOperation();
	operation->request = request;
	operationQueue.Insert(operation);
}

CompositorQueue::CompositorOperation::~CompositorOperation()
{
	SafeRelease(compositor);
}

void CompositorQueue::CompositorOperation::SetTextureCompositor(ITextureCompositor* newCompositor)
{
	SafeAssign(&compositor, newCompositor);
}
