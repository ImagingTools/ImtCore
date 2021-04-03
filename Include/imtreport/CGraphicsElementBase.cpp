#include <imtreport/CGraphicsElementBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtreport
{


// public methods

// reimplemented (IDrawElement)

IDrawElement::GraphicsAttributes CGraphicsElementBase::GetGraphicsAttributes() const
{
	return m_graphicsAttributes;
}


void CGraphicsElementBase::SetGraphicsAttributes(const GraphicsAttributes& graphicsAttributes)
{
	istd::CChangeNotifier changeNotifier(this);

	m_graphicsAttributes = graphicsAttributes;
}


// reimplemented (iser::ISerializeable)

bool CGraphicsElementBase::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? NULL : this);

	static iser::CArchiveTag fillColorTag("FillColor", "Color used for object filling", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fillColorTag);

	QRgb fillcColorValue = m_graphicsAttributes.fillColor.rgba();
	retVal = retVal && archive.Process(fillcColorValue);
	if (retVal && !archive.IsStoring())
	{
		m_graphicsAttributes.fillColor.setRgba(fillcColorValue);
	}

	retVal = retVal && archive.EndTag(fillColorTag);

	static iser::CArchiveTag strokeColorTag("StrokeColor", "Color used for object stroking", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(strokeColorTag);

	QRgb strokeColorValue = m_graphicsAttributes.strokeColor.rgba();
	retVal = retVal && archive.Process(strokeColorValue);
	if (retVal && !archive.IsStoring()){
		m_graphicsAttributes.strokeColor.setRgba(strokeColorValue);
	}

	retVal = retVal && archive.EndTag(strokeColorTag);

	static iser::CArchiveTag strokeWidthTag("StrokeWidth", "Width used for object stroking", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(strokeWidthTag);
	retVal = retVal && archive.Process(m_graphicsAttributes.strokeWidth);
	retVal = retVal && archive.EndTag(strokeWidthTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CGraphicsElementBase::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGraphicsElementBase* sourcePtr = dynamic_cast<const CGraphicsElementBase*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_graphicsAttributes = sourcePtr->m_graphicsAttributes;

		return true;
	}

	return false;
}


} // namespace imtreport


