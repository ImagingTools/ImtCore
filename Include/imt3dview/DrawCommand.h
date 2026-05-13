// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtGui/QMatrix4x4>

// ImtCore includes
#include <imt3dview/IRenderResource.h>
#include <imt3dview/Material.h>


namespace imt3dview
{


/**
	Backend-neutral description of a single draw call. Built by IScene3dItem::Render
	and consumed by IRenderBackend::Draw.
*/
struct DrawCommand
{
	IRenderResourcePtr geometry;
	PrimitiveType primitive = PT_TRIANGLES;
	int indexCount = 0;
	int indexOffset = 0;
	Material material;
	QMatrix4x4 modelMatrix;
};


} // namespace imt3dview
