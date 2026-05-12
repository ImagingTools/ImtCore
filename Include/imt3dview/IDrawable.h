// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QPainter>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imt3dview
{


class IRenderBackend;


/**
	Basic drawable object interface. Backend-neutral; concrete render work is delegated
	to an IRenderBackend instance attached via OnAttachBackend.
*/
class IDrawable: virtual public istd::IPolymorphic
{
public:
	/**
		Attach the drawable to a render backend (or detach by passing nullptr).
		Backend-owned resources should be (re)created here.
	*/
	virtual void OnAttachBackend(IRenderBackend* backendPtr) = 0;

	/**
		Render the drawable using the given backend (which must be the same instance
		previously passed to OnAttachBackend).
	*/
	virtual void Render(IRenderBackend& backend) = 0;

	/**
		Optional 2D painter overlay (axis labels, info boxes, ...) drawn on top of the
		3D scene by the host. This is intentionally outside the IRenderBackend pipeline.
	*/
	virtual void DrawOverlay(QPainter& painter) = 0;
};


} // namespace imt3dview
