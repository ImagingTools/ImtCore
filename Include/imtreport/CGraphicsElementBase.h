// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtreport/IDrawElement.h>


namespace imtreport
{


/**
	Basic implementation of a drawable element.
*/
class CGraphicsElementBase: virtual public IDrawElement
{
public:
	// reimplemented (IDrawElement)
	virtual GraphicsAttributes GetGraphicsAttributes() const override;
	virtual void SetGraphicsAttributes(const GraphicsAttributes& graphicsAttributes) override;

	// reimplemented (iser::ISerializeable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	GraphicsAttributes m_graphicsAttributes;
};


} // namespace imtreport


