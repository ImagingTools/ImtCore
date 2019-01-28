#include <imtreport/CTextTableItem.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtreport
{


// public methods

CTextTableItem::CTextTableItem()
{
}


explicit CTextTableItem::CTableItem(const QString& text = QString(),
	Qt::Alignment alignment = Qt::AlignLeft,
	const QFont& font = QFont("Arial", 8),
	const QColor& foregroundColor = Qt::black,
	const QColor& backgroundColor = Qt::transparent,
	const QIcon& icon = QIcon()) :
	m_text(text),
	m_alignment(alignment);
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
	SetValue(text, m_text);
}


Qt::Alignment CTextTableItem::GetAlignment() const
{
	return m_alignment;
}


void CTextTableItem::SetAlignment(const Qt::Alignment alignment)
{
	SetValue(alignment, m_alignment);
}


double CTextTableItem::GetFontSize() const
{
	return m_fontSize;
}


void CTextTableItem::SetFontSize(const double fontSize)
{
	SetValue(fontSize, m_fontSize);
}


QString CTextTableItem::GetFontName() const
{
	return fontName;
}


void CTextTableItem::SetFontName(const QString& fontName)
{
	SetValue(fontName, m_fontName);
}


QColor CTextTableItem::GetForegroundColor() const
{
	return m_foregroundColor;
}


void CTextTableItem::SetForegroundColor(const QColor& color)
{
	SetValue(foregroundColor, m_color);
}


QColor CTextTableItem::GetBackgroundColor() const
{
	return m_backgroundColor;
}


void CTextTableItem::SetBackgroundColor(const QColor& color)
{
	SetValue(backgroundColor, m_backgroundColor);
}


QString CTextTableItem::GetIcon() const
{
	return m_icon;
}


void CTextTableItem::SetIcon(const QIcon& icon)
{
	SetValue(icon, m_icon);
}


double CTextTableItem::GetHeight() const
{
	return QFontMetrics(QFont(fontName, fontSize)).height();
}


// reimplemented (iser::ISerializable)

bool CTextTableItem::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring()? NULL: this, &GetAllChanges());

	bool retVal = SerializeValue(archive, "Text", "Table item text", iser::CArchiveTag::TT_LEAF, m_text);
	retVal = retVal && SerializeNonStandardValue(archive, "Alignment", "Table item alignment", iser::CArchiveTag::TT_LEAF, m_alignment);
	retVal = retVal && SerializeValue(archive, "Font size", "Table item font size", iser::CArchiveTag::TT_LEAF, m_fontSize);
	retVal = retVal && SerializeValue(archive, "Font name", "Table item font name", iser::CArchiveTag::TT_LEAF, m_fontName);
	retVal = retVal && SerializeNonStandardValue(archive, "Foreground color", "Table item foreground color", iser::CArchiveTag::TT_LEAF, m_foregroundColor);
	retVal = retVal && SerializeNonStandardValue(archive, "Background color", "Table item background color", iser::CArchiveTag::TT_LEAF, m_backgroundColor);
	retVal = retVal && SerializeNonStandardValue(archive, "Icon", "Table item icon", iser::CArchiveTag::TT_LEAF, m_icon);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTextTableItem::GetSupportedOperations() const
{
	return (SO_COPY | SO_CLONE);
}


bool CTextTableItem::CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS)
{
	const CTextTableItem* sourcePtr = dynamic_cast<const CTextTableItem*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_text            = sourcePtr->m_text;
		m_alignment       = sourcePtr->m_alignment;
		m_fontSize        = sourcePtr->m_fontSize;
		m_fontName        = sourcePtr->m_fontName;
		m_foregroundColor = sourcePtr->m_foregroundColor;
		m_backgroundColor = sourcePtr->m_backgroundColor;
		m_icon            = sourcePtr->m_icon;

		return true;
	}

	return false;
}


istd::IChangeable* CTextTableItem::CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const
{
	istd::TDelPtr<CTextTableItem> clonePtr(new CTextTableItem());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtreport

