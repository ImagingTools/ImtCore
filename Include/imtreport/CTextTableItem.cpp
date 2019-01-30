#include <imtreport/CTextTableItem.h>


// Qt includes
#include <QtGui/QFontMetrics>

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
			QString fontName,
			double fontSize,
			const QColor& foregroundColor,
			const QColor& backgroundColor,
			const QIcon& icon)
	:m_text(text),
	m_alignment(alignment),
	m_fontSize(fontSize),
	m_fontName(fontName),
	m_foregroundColor(foregroundColor),
	m_backgroundColor(backgroundColor),
	m_icon(icon)
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


double CTextTableItem::GetFontSize() const
{
	return m_fontSize;
}


void CTextTableItem::SetFontSize(const double fontSize)
{
	if (m_fontSize != fontSize){
		istd::CChangeNotifier changeNotifier(this);
		
		m_fontSize = fontSize;
	}
}


QString CTextTableItem::GetFontName() const
{
	return m_fontName;
}


void CTextTableItem::SetFontName(const QString& fontName)
{
	if (m_fontName != fontName){
		istd::CChangeNotifier changeNotifier(this);
		
		m_fontName = fontName;
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


QIcon CTextTableItem::GetIcon() const
{
	return m_icon;
}


void CTextTableItem::SetIcon(const QIcon& icon)
{
	istd::CChangeNotifier changeNotifier(this);

	m_icon = icon;
}


double CTextTableItem::GetHeight() const
{
	return QFontMetrics(QFont(m_fontName, m_fontSize)).height();
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

	retVal = retVal && SerializeValue(archive, "Font size", "Table item font size", iser::CArchiveTag::TT_LEAF, m_fontSize);
	retVal = retVal && SerializeValue(archive, "Font name", "Table item font name", iser::CArchiveTag::TT_LEAF, m_fontName);
/*
	static iser::CArchiveTag tagForegroundColor("Foreground color", "Table item foreground color", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagForegroundColor);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeQtObject(archive, m_foregroundColor);
	retVal = retVal && archive.EndTag(tagForegroundColor);

	static iser::CArchiveTag tagBackgroundColor("Background color", "Table item background color", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagBackgroundColor);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeQtObject(archive, m_backgroundColor);
	retVal = retVal && archive.EndTag(tagBackgroundColor);

	static iser::CArchiveTag tagIcon("Icon", "Table item icon", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagIcon);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeQtObject(archive, m_icon);
	retVal = retVal && archive.EndTag(tagIcon);
*/
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
		m_fontSize = sourcePtr->m_fontSize;
		m_fontName = sourcePtr->m_fontName;
		m_foregroundColor = sourcePtr->m_foregroundColor;
		m_backgroundColor = sourcePtr->m_backgroundColor;
		m_icon = sourcePtr->m_icon;

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

