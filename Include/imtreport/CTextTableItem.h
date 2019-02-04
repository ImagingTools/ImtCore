#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QColor>

// ACF includes
#include <iser/ISerializable.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iimg/CBitmap.h>

// ImtCore includes
#include <imtreport/CFont.h>


namespace imtreport
{


class CTextTableItem: virtual public iser::ISerializable
{
public:
	CTextTableItem(
				const QString& text = QString(),
				Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter,
				CFont font = CFont("Arial", 2.5),
				const QColor& foregroundColor = Qt::black,
				const QColor& backgroundColor = Qt::transparent,
				const iimg::CBitmap& image = iimg::CBitmap());

	QString GetText() const;
	void SetText(const QString& text);

	Qt::Alignment GetAlignment() const;
	void SetAlignment(const Qt::Alignment alignment);

	const CFont& GetFont() const;
	void SetFont(const CFont& font);

	QColor GetForegroundColor() const;
	void SetForegroundColor(const QColor& color);

	QColor GetBackgroundColor() const;
	void SetBackgroundColor(const QColor& color);

	const iimg::CBitmap& GetImage() const;
	void SetImage(const iimg::CBitmap& image);

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented istd::IChangeable
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	template <typename T>
	bool SerializeValue(iser::IArchive& archive, const QByteArray& tagId, const QByteArray& tagComment, iser::CArchiveTag::TagType tagType, T& value)
	{
		static iser::CArchiveTag tag(tagId, tagComment, tagType);

		bool retVal = archive.BeginTag(tag);
		retVal = retVal && archive.Process(value);
		retVal = retVal && archive.EndTag(tag);

		return retVal;
	}

private:
	QString m_text;
	Qt::Alignment m_alignment;
	CFont m_font;
	QColor m_foregroundColor;
	QColor m_backgroundColor;
	iimg::CBitmap m_image;
};


} // namespace imtreport


