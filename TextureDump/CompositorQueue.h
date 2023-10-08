#pragma once

#include "strtools.h"
#include "utlqueue.h"
#include "utlstring.h"

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
		CUtlString paintKitDefName;
		int itemDefIndex;
		CUtlString itemDefName;
		int wear;
		int team;
		uint64 seed;
		int width;
		int height;
		CUtlString outputDir;
		CUtlString outputName;

		const class CPaintKitDefinition* paintKitDef;
		const class CPaintKitItemDefinition* itemDef;

		CUtlString ToDebugString();
	};

	void EnqueueRequest(const CompositorRequest& request);

	static CompositorQueue* Singleton();

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

