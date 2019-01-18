#include <imtreport/CGraphicsElementBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtreport
{


CGraphicsElementBase::CGraphicsElementBase()
	:m_fillColor(),
	m_strokeColor(),
	m_strokeWidth(-1),
	m_alignment(Alignment::None)
{
}


// reimplemented (IGraphicsElement)

QColor CGraphicsElementBase::GetFillColor() const
{
	return m_fillColor;
}


void CGraphicsElementBase::SetFillColor(const QColor& fillColor)
{
	if (m_fillColor != fillColor)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_fillColor = fillColor;
	}
}


QColor CGraphicsElementBase::GetStrokeColor() const
{
	return m_strokeColor;
}


void CGraphicsElementBase::SetStrokeColor(const QColor& strokeColor)
{
	if (m_strokeColor != strokeColor)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_strokeColor = strokeColor;
	}
}


double CGraphicsElementBase::GetStrokeWidth() const
{
	return m_strokeWidth;
}


void CGraphicsElementBase::SetStrokeWidth(double strokeWidth)
{
	if (m_strokeWidth != strokeWidth)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_strokeWidth = strokeWidth;
	}
}


IGraphicsElement::Alignment CGraphicsElementBase::GetAlignment() const
{
	return m_alignment;
}


void CGraphicsElementBase::SetAlignment(const Alignment alignment)
{
	if (m_alignment != alignment)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_alignment = alignment;
	}
}


// reimplemented (iser::ISerializeable)

bool CGraphicsElementBase::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? NULL : this);

	static iser::CArchiveTag fillColorTag("FillColor", "Color used for object filling", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fillColorTag);

	QRgb fillcColorValue = m_fillColor.rgba();
	retVal = retVal && archive.Process(fillcColorValue);
	if (retVal && !archive.IsStoring())
	{
		m_fillColor.setRgba(fillcColorValue);
	}

	retVal = retVal && archive.EndTag(fillColorTag);

	static iser::CArchiveTag strokeColorTag("StrokeColor", "Color used for object stroking", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(strokeColorTag);

	QRgb strokeColorValue = m_strokeColor.rgba();
	retVal = retVal && archive.Process(strokeColorValue);
	if (retVal && !archive.IsStoring()){
		m_strokeColor.setRgba(strokeColorValue);
	}

	retVal = retVal && archive.EndTag(strokeColorTag);

	static iser::CArchiveTag strokeWidthTag("StrokeWidth", "Width used for object stroking", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(strokeWidthTag);
	retVal = retVal && archive.Process(m_strokeWidth);
	retVal = retVal && archive.EndTag(strokeWidthTag);

	int alignment = m_alignment;

	static iser::CArchiveTag alignmentTag("Alignment", "Alignment", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(alignmentTag);
	retVal = retVal && archive.Process(alignment);
	retVal = retVal && archive.EndTag(alignmentTag);
	if (retVal && !archive.IsStoring()) {
		m_alignment = static_cast<IGraphicsElement::Alignment> (alignment);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CGraphicsElementBase::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGraphicsElementBase* sourcePtr = dynamic_cast<const CGraphicsElementBase*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_fillColor = sourcePtr->m_fillColor;
		m_strokeColor = sourcePtr->m_strokeColor;
		m_strokeWidth = sourcePtr->m_strokeWidth;
		m_alignment = sourcePtr->m_alignment;

		return true;
	}

	return false;
}


} // namespace imtreport


