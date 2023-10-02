#pragma once

#include "strtools.h"
#include "UtlQueue.h"

class ITextureCompositor;

class CompositorQueue
{
public:
	~CompositorQueue();
	void Update();

	struct CompositorRequest
	{
	public:
		int paintKitDefIndex;
		int itemDefIndex;
		int wear;
		int team;
		uint64 seed;
	};

	void EnqueueRequest(const CompositorRequest& request);

private:
	class CompositorOperation
	{
	public:
		~CompositorOperation();
		void SetTextureCompositor(ITextureCompositor* newCompositor);
		inline ITextureCompositor* GetTextureCompositor() const { return compositor; }

		CompositorRequest request;
	private:
		ITextureCompositor* compositor;
	};

	CUtlQueue<CompositorOperation*> operationQueue;
};

