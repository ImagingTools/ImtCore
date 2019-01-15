#include <imtreports/CTextLabel.h>


// ACF includes
#include "istd/TDelPtr.h"
#include "istd/CChangeNotifier.h"
#include "iser/CArchiveTag.h"


namespace imtreports
{


// public methods

CTextLabel::CTextLabel()
	:m_fontSize(8),
	m_fontName("Arial")
{
}


const QString& CTextLabel::GetFontName() const
{
	return m_fontName;
}


void CTextLabel::SetFontName(const QString& fontName)
{
	if (m_fontName != fontName)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_fontName = fontName;
	}
}


double CTextLabel::GetFontSize() const
{
	return m_fontSize;
}


void CTextLabel::SetFontSize(double fontSize)
{
	if (m_fontSize != fontSize)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_fontSize = fontSize;
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


// reimplemented (iser::ISerializable)

bool CTextLabel::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag fontNameTag("FontName", "Text label font name");
	static iser::CArchiveTag fontSizeTag("FontSize", "Text label font size");
	static iser::CArchiveTag rectTag("Rectangle", "Text label rectangle");

	istd::CChangeNotifier notifier(archive.IsStoring()? NULL: this, &GetAllChanges());

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	retVal = retVal && archive.BeginTag(fontNameTag);
	retVal = retVal && archive.Process(m_fontName);
	retVal = retVal && archive.EndTag(fontNameTag);

	retVal = retVal && archive.BeginTag(fontSizeTag);
	retVal = retVal && archive.Process(m_fontSize);
	retVal = retVal && archive.EndTag(fontSizeTag);

	retVal = retVal && archive.BeginTag(rectTag);
	retVal = retVal && m_rectangle.Serialize(archive);
	retVal = retVal && archive.EndTag(rectTag);

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

		m_fontName = labelPtr->GetFontName();
		m_fontSize = labelPtr->GetFontSize();
		m_rectangle = labelPtr->GetRectangle();

		BaseClass::CopyFrom(object, mode);

		return true;
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


} // namespace imtreports


