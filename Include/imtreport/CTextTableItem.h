#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QIcon>

// ACF includes
#include <iser/ISerializable.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtreport
{


class CTextTableItem: virtual public iser::ISerializable
{
public:
	CTextTableItem(
				const QString& text = QString(),
				Qt::Alignment alignment = Qt::AlignLeft,
				QString fontName = "Arial",
				double fontSize = 2.5,
				const QColor& foregroundColor = Qt::black,
				const QColor& backgroundColor = Qt::transparent,
				const QIcon& icon = QIcon());

	QString GetText() const;
	void SetText(const QString& text);

	Qt::Alignment GetAlignment() const;
	void SetAlignment(const Qt::Alignment alignment);

	double GetFontSize() const;
	void SetFontSize(const double fontSize);

	QString GetFontName() const;
	void SetFontName(const QString& fontName);

	QColor GetForegroundColor() const;
	void SetForegroundColor(const QColor& color);

	QColor GetBackgroundColor() const;
	void SetBackgroundColor(const QColor& color);

	QIcon GetIcon() const;
	void SetIcon(const QIcon& icon);

	double GetHeight() const;

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

	template <typename T>
	bool SerializeNonStandardValue(iser::IArchive& archive, const QByteArray& tagId, const QByteArray& tagComment, iser::CArchiveTag::TagType tagType, T& value)
	{
		static iser::CArchiveTag tag(tagId, tagComment, tagType);

		T tempValue = value;

		bool retVal = archive.BeginTag(tag);
		retVal = retVal && archive.Process(tempValue);
		retVal = retVal && archive.EndTag(tag);

		if (retVal && !archive.IsStoring()){
			value = tempValue;
		}

		return retVal;
	}

private:
	QString m_text;
	Qt::Alignment m_alignment;
	double m_fontSize;
	QString m_fontName;
	QColor m_foregroundColor;
	QColor m_backgroundColor;
	QIcon m_icon;
};


} // namespace imtreport


