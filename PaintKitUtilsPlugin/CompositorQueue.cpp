#include "CompositorQueue.h"
#include "materialsystem/itexturecompositor.h"
#include "materialsystem/imaterialsystem.h" 
#include "bitmap/imageformat.h" // For IMAGE_MAX_DIM
#include "refcount.h"
#include "UtlString.h"
#include "fileio.h"
#include "CProtoBufScriptObjectDefinitionManager.h"
#include <exception>
#include <stdexcept>
#include "FileUtils.h"
#include "KeyValues.h"

CompositorQueue::~CompositorQueue()
{
	CancelAll();
}

void CompositorQueue::CancelAll()
{
	while (!operationQueue.IsEmpty())
	{
		auto operation = operationQueue.RemoveAtHead();
		CompositorResult compositorResult;
		compositorResult.resultTexture = NULL;
		compositorResult.request = &operation->request;
		compositorResult.exception = NULL;
		operation->request.onComplete(compositorResult);
		delete operation;
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

			const auto compositeFlags = TextureCompositeCreateFlags_t::TEX_COMPOSITE_CREATE_FLAGS_NO_COMPRESSION;

			currentOperation->SetTextureCompositor(
				materials->NewTextureCompositor(request.GetWidth(), request.GetHeight(), request.name.Get(), request.team, request.seed, const_cast<KeyValues*>(&request.GetStageDesc()), compositeFlags)
			);

			// If we failed to get a new compositor, bail on this operation.
			if (currentOperation->GetTextureCompositor() == NULL)
			{
				CUtlString str;
				str.Format("IMaterialSystem::NewTextureCompositor(w: %d, h: %d, pCompositeName: %s, nTeamNum: %d, randomSeed: %ull, stageDesc: 0x%p, texCompositeCreateFlags: %d) failed!",
					request.GetWidth(),
					request.GetHeight(),
					request.name.Get(),
					request.team,
					request.seed, 
					&request.GetStageDesc(),
					compositeFlags
				);
				throw std::runtime_error(str);
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

			// Call the request's onComplete to give the submitter the results of the operation.
			CompositorResult result;
			result.exception = NULL;
			result.request = &currentOperation->request;
			result.resultTexture = resultTexture;
			currentOperation->request.onComplete(result);

			// Release the generated texture. If the caller wants to keep it, they'll need to add their own refs.
			resultTexture->Release();

			// Pop this operation off the queue.
			delete operationQueue.RemoveAtHead();
		} break;
		}
	}
	catch (std::exception e)
	{
		// Call the request's onComplete to let the submitter know that the operation failed.
		CompositorResult result;
		result.exception = &e;
		result.request = &currentOperation->request;
		result.resultTexture = currentOperation->GetTextureCompositor()->GetResultTexture();
		currentOperation->request.onComplete(result);

		// Pop this operation off the queue.
		Warning("Encountered error in compositor operation for request %s, discarding. Error: %s\n", currentOperation->request.ToDebugString().Get(), e.what());
		delete operationQueue.RemoveAtHead();
	}
}

void CompositorQueue::EnqueueRequest(CompositorRequest&& request)
{
	CompositorOperation* operation = new CompositorOperation();
	operation->request = std::move(request);
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

CompositorQueue::CompositorRequest::CompositorRequest() :
	name(""),
	team(0),
	seed(0),
	width(IMAGE_MAX_DIM),
	height(IMAGE_MAX_DIM),
	onComplete(NULL)
{
	stageDesc = new KeyValues("DUMMY");
}

CompositorQueue::CompositorRequest::~CompositorRequest() {
	if (stageDesc)
	{
		stageDesc->deleteThis();
	}
}

CUtlString CompositorQueue::CompositorRequest::ToDebugString()
{
	/*
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
	*/
	return "{ NOT_IMPLEMENTED }";
}

void CompositorQueue::CompositorRequest::SetStageDesc(const KeyValues* newValue)
{
	*stageDesc = *newValue;
}

const KeyValues& CompositorQueue::CompositorRequest::GetStageDesc() const {
	return *stageDesc;
}

int CompositorQueue::CompositorRequest::GetWidth() const
{
	return width;
}

void CompositorQueue::CompositorRequest::SetWidth(int newWidth)
{
	if (newWidth <= 0)
	{
		CUtlString msg;
		msg.Format("width must be positive, was %d", newWidth);
		throw std::runtime_error(msg);
	}
	if (IMAGE_MAX_DIM < newWidth)
	{
		CUtlString msg;
		msg.Format("width must be less than IMAGE_MAX_DIM (%d), was %d", IMAGE_MAX_DIM, newWidth);
		throw std::runtime_error(msg);
	}
	width = newWidth;
}

int CompositorQueue::CompositorRequest::GetHeight() const
{
	return height;
}

void CompositorQueue::CompositorRequest::SetHeight(int newHeight)
{
	if (newHeight <= 0)
	{
		CUtlString msg;
		msg.Format("height must be positive, was %d", newHeight);
		throw std::runtime_error(msg);
	}
	if (IMAGE_MAX_DIM < newHeight)
	{
		CUtlString msg;
		msg.Format("height must be less than IMAGE_MAX_DIM (%d), was %d", IMAGE_MAX_DIM, newHeight);
		throw std::runtime_error(msg);
	}
	height = newHeight;
}
