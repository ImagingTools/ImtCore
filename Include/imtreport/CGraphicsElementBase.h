#pragma once


// ImtCore includes
#include <imtreport/IGraphicsElement.h>


namespace imtreport
{


/**
	Interface for describing a graphic element.
*/
class CGraphicsElementBase: virtual public IGraphicsElement
{
public:
	// reimplemented (IGraphicsElement)
	virtual GraphicsAttributes GetGraphicsAttributes() const override;
	virtual void SetGraphicsAttributes(const GraphicsAttributes& graphicsAttributes) override;

	// reimplemented (iser::ISerializeable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);

protected:
	GraphicsAttributes m_graphicsAttributes;
};


} // namespace imtreport


