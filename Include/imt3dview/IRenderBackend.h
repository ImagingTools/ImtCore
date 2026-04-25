// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <cstddef>
#include <cstdint>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imt3dview/IRenderResource.h>
#include <imt3dview/SDrawCommand.h>
#include <imt3dview/SSceneState.h>
#include <imt3dview/SVertexLayout.h>


namespace imt3dview
{


/**
	Backend-neutral 3D-render abstraction. Concrete implementations (e.g. an OpenGL backend)
	encapsulate program/pipeline lifetime, buffer management and per-draw uniform setup so that
	IDrawable/IScene3dItem implementations can be expressed purely in terms of geometry,
	primitive type, material and model matrix.
*/
class IRenderBackend: virtual public istd::IPolymorphic
{
public:
	/**
		Initialize backend resources (programs, pipelines, ...).
		Returns true on success.
	*/
	virtual bool Initialize() = 0;

	/**
		Release backend resources. Outstanding IRenderResource handles become invalid.
	*/
	virtual void Shutdown() = 0;

	/**
		Begin a frame: clear, apply scene-wide state and global uniforms.
	*/
	virtual void BeginFrame(const SSceneState& sceneState) = 0;

	/**
		Finish a frame.
	*/
	virtual void EndFrame() = 0;

	/**
		Create a new geometry resource for the given vertex layout.
	*/
	virtual IRenderResourcePtr CreateGeometry(const SVertexLayout& layout) = 0;

	/**
		Upload vertex data and indices into a previously created geometry resource.
	*/
	virtual void UpdateGeometry(
				IRenderResource& resource,
				const void* vertexData,
				size_t vertexBytes,
				const uint32_t* indices,
				size_t indexCount) = 0;

	/**
		Refresh just the vertex data of a geometry resource (no index changes).
	*/
	virtual void RefreshVertices(
				IRenderResource& resource,
				const void* vertexData,
				size_t vertexBytes) = 0;

	/**
		Issue a single draw call: bind geometry, set per-draw uniforms, draw primitives.
	*/
	virtual void Draw(const SDrawCommand& command) = 0;

	/**
		Explicitly destroy a backend-owned resource. Optional; releasing the last shared_ptr
		reference also lets the backend free its data.
	*/
	virtual void DestroyResource(IRenderResource& resource) = 0;
};


} // namespace imt3dview
