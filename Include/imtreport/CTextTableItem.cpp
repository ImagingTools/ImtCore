#include <imtreport/CTextTableItem.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtreport
{


// public methods

CTextTableItem::CTextTableItem(
			const QString& text,
			Qt::Alignment alignment,
			CFont font,
			const QColor& foregroundColor,
			const QColor& backgroundColor,
			const iimg::CBitmap& image)
	:m_text(text),
	m_alignment(alignment),
	m_font(font),
	m_foregroundColor(foregroundColor),
	m_backgroundColor(backgroundColor),
	m_image(image)
{
}


QString CTextTableItem::GetText() const
{
	return m_text;
}


void CTextTableItem::SetText(const QString& text)
{
	if (m_text != text){
		istd::CChangeNotifier changeNotifier(this);
		
		m_text = text;
	}
}


Qt::Alignment CTextTableItem::GetAlignment() const
{
	return m_alignment;
}


void CTextTableItem::SetAlignment(const Qt::Alignment alignment)
{
	if (m_alignment != alignment){
		istd::CChangeNotifier changeNotifier(this);
		
		m_alignment = alignment;
	}
}


const CFont& CTextTableItem::GetFont() const
{
	return m_font;
}


void CTextTableItem::SetFont(const CFont& font)
{
	if (!m_font.IsEqual(font)){
		istd::CChangeNotifier changeNotifier(this);
		
		m_font = font;
	}
}


QColor CTextTableItem::GetForegroundColor() const
{
	return m_foregroundColor;
}


void CTextTableItem::SetForegroundColor(const QColor& color)
{
	if (m_foregroundColor != color){
		istd::CChangeNotifier changeNotifier(this);
		
		m_foregroundColor = color;
	}
}


QColor CTextTableItem::GetBackgroundColor() const
{
	return m_backgroundColor;
}


void CTextTableItem::SetBackgroundColor(const QColor& color)
{
	if (m_backgroundColor != color){
		istd::CChangeNotifier changeNotifier(this);
		
		m_backgroundColor = color;
	}
}


const iimg::CBitmap& CTextTableItem::GetImage() const
{
	return m_image;
}


void CTextTableItem::SetImage(const iimg::CBitmap& image)
{
	istd::CChangeNotifier changeNotifier(this);

	m_image = image;
}


// reimplemented (iser::ISerializable)

bool CTextTableItem::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring()? NULL: this, &GetAllChanges());

	bool retVal = SerializeValue(archive, "Text", "Table item text", iser::CArchiveTag::TT_LEAF, m_text);

	static iser::CArchiveTag tagAlignment("Alignment", "Table item alignment", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagAlignment);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<Qt::Alignment, NULL, NULL>(archive, m_alignment);
	retVal = retVal && archive.EndTag(tagAlignment);

	static iser::CArchiveTag tagFont("Font", "Table item font", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagFont);
	retVal = retVal && m_font.Serialize(archive);
	retVal = retVal && archive.EndTag(tagFont);

	//static iser::CArchiveTag tagForegroundColor("ForegroundColor", "Table item foreground color", iser::CArchiveTag::TT_LEAF);
	//retVal = retVal && archive.BeginTag(tagForegroundColor);
	//retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeQColor(archive, m_foregroundColor);
	//retVal = retVal && archive.EndTag(tagForegroundColor);

	//static iser::CArchiveTag tagBackgroundColor("BackgroundColor", "Table item background color", iser::CArchiveTag::TT_LEAF);
	//retVal = retVal && archive.BeginTag(tagBackgroundColor);
	//retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeQColor(archive, m_backgroundColor);
	//retVal = retVal && archive.EndTag(tagBackgroundColor);

	static iser::CArchiveTag tagImage("Image", "Table item image", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagImage);
	retVal = retVal && m_image.Serialize(archive);
	retVal = retVal && archive.EndTag(tagImage);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTextTableItem::GetSupportedOperations() const
{
	return (SO_COPY | SO_CLONE);
}


bool CTextTableItem::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTextTableItem* sourcePtr = dynamic_cast<const CTextTableItem*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_text = sourcePtr->m_text;
		m_alignment = sourcePtr->m_alignment;
		m_font = sourcePtr->m_font;
		m_foregroundColor = sourcePtr->m_foregroundColor;
		m_backgroundColor = sourcePtr->m_backgroundColor;
		m_image = sourcePtr->m_image;

		return true;
	}

	return false;
}


istd::IChangeable* CTextTableItem::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CTextTableItem> clonePtr(new CTextTableItem());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtreport

