#pragma once

#include "strtools.h" // Prerequisite for "utlqueue.h"
#include "utlqueue.h"
#include "utlstring.h"

#include <functional>

class ITexture;
class ITextureCompositor;
class KeyValues;

/// <summary>
/// A class which accepts requests to generate composited textures and processes them in order.
/// </summary>
class CompositorQueue
{
public:
	~CompositorQueue();

	void CancelAll();

	/// <summary>
	/// Runs a processing step on the queue.
	/// </summary>
	void Update();

	class CompositorRequest;

	struct CompositorResult
	{
		const CompositorRequest* request;
		const std::exception* exception;
		ITexture* resultTexture;
	};

	/// <summary>
	/// Represents a request for textures to be generated.
	/// </summary>
	class CompositorRequest
	{
	public:
		CompositorRequest();
		~CompositorRequest();
		CUtlString ToDebugString();

		void SetStageDesc(const KeyValues* newValue);
		const KeyValues& GetStageDesc() const;

		// No copy. If any copy operator is implemented, it'll need to account for the stageDesc field since this type has ownership of it.
		CompositorRequest& operator=(const CompositorRequest& other) = delete;

		// Move is perfectly OK!
		CompositorRequest& operator=(CompositorRequest&& other) = default;

		typedef bool (*RequestCallback_t)(const CompositorResult& result);

		int GetWidth() const;
		void SetWidth(int newWidth);

		int GetHeight() const;
		void SetHeight(int newHeight);

	public:
		CUtlString name;
		int team;
		uint64 seed;
		std::function<void(const CompositorResult&)> onComplete;

	private:
		int width;
		int height;
		KeyValues* stageDesc;
	};



	void EnqueueRequest(CompositorRequest&& request);

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
