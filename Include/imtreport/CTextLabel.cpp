#include <imtreport/CTextLabel.h>


// ACF includes
#include "istd/TDelPtr.h"
#include "istd/CChangeNotifier.h"
#include "iser/CArchiveTag.h"


namespace imtreport
{


// public methods

CTextLabel::CTextLabel()
	:m_font("Arial", 8.0),
	m_alignment(Qt::AlignLeft)
{
}


const CFont& CTextLabel::GetFont() const
{
	return m_font;
}


void CTextLabel::SetFont(const CFont& font)
{
	if (!m_font.IsEqual(font))
	{
		istd::CChangeNotifier changeNotifier(this);

		m_font = font;
	}
}


const i2d::CRectangle& CTextLabel::GetRectangle() const
{
	return m_rectangle;
}


void CTextLabel::SetRectangle(const i2d::CRectangle& rectangle)
{
	if (m_rectangle != rectangle)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_rectangle = rectangle;
	}
}


Qt::Alignment CTextLabel::GetAlignment() const
{
	return m_alignment;
}


void CTextLabel::SetAlignment(const Qt::Alignment alignment)
{
	if (m_alignment != alignment)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_alignment = alignment;
	}
}


// reimplemented (iser::ISerializable)

bool CTextLabel::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag fontTag("Font", "Text label font", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag rectTag("Rectangle", "Text label rectangle", iser::CArchiveTag::TT_GROUP);
	static iser::CArchiveTag alignmentTag("Alignment", "Alignment", iser::CArchiveTag::TT_LEAF);

	istd::CChangeNotifier notifier(archive.IsStoring()? NULL: this, &GetAllChanges());

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	retVal = retVal && archive.BeginTag(fontTag);
	retVal = retVal && m_font.Serialize(archive);
	retVal = retVal && archive.EndTag(fontTag);

	retVal = retVal && archive.BeginTag(rectTag);
	retVal = retVal && m_rectangle.Serialize(archive);
	retVal = retVal && archive.EndTag(rectTag);

	int alignment = m_alignment;

	retVal = retVal && archive.BeginTag(alignmentTag);
	retVal = retVal && archive.Process(alignment);
	retVal = retVal && archive.EndTag(alignmentTag);
	if (retVal && !archive.IsStoring()) {
		m_alignment = static_cast<Qt::Alignment>(alignment);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTextLabel::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE;
}


bool CTextLabel::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CTextLabel* labelPtr = dynamic_cast<const CTextLabel*>(&object);
	if (labelPtr != NULL)
	{
		istd::CChangeNotifier notifier(this);

		bool retVal = m_font.CopyFrom(labelPtr->GetFont());

		if (retVal){
			m_rectangle = labelPtr->GetRectangle();
			m_alignment = labelPtr->GetAlignment();

			retVal = BaseClass::CopyFrom(object, mode);
		}

		return retVal;
	}

	return false;
}


istd::IChangeable* CTextLabel::CloneMe(CompatibilityMode mode) const 
{
	istd::TDelPtr<CTextLabel> clonePtr(new CTextLabel);

	if (clonePtr->CopyFrom(*this, mode))
	{
		return clonePtr.PopPtr();
	}

	return NULL;
}


} // namespace imtreport


