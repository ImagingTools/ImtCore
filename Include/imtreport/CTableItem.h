#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QColor>

// ImtCore includes
#include <imtreport/ITableItem>


namespace imtreport
{


class CTableItem: public virtual ITableItem
{
public:
	explicit CTableItem(const QString& text);

	// reimplemented (ITableItem)
	QString GetText() const override;
	void SetText(const QString& text) override;

	Qt::Alignment GetAlignment() const override;
	void SetAlignment(const Qt::Alignment alignment) override;

	double GetFontSize() const override;
	void SetFontSize(const double fontSize) override;

	QString GetFontName() const override;
	void SetFontName(const QString& fontName) override;

	QColor GetForegroundColor() const override;
	void SetForegroundColor(const QColor& color) override;

	QColor GetBackgroundColor() const override;
	void SetBackgroundColor(const QColor& color) override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented istd::IChangeable
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	template <typename T>
	void SetValue(const T& srcValue, T& dstValue)
	{
		if (srcValue != dstValue){
			istd::CChangeNotifier changeNotifier(this);

			dstValue = srcValue;
		}
	}

	template <typename T>
	bool SerializeValue(iser::IArchive& archive, const QByteArray& tagId, const QByteArray& tagComment, TagType tagType, T& value)
	{
		static iser::CArchiveTag tag(tagId, tagComment, tagType);

		bool retVal = archive.BeginTag(tag);
		retVal = retVal && archive.Process(value);
		retVal = retVal && archive.EndTag(tag);

		return retVal;
	}

	template <typename T>
	bool SerializeNonStandardValue(iser::IArchive& archive, const QByteArray& tagId, const QByteArray& tagComment, TagType tagType, T& value)
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
	Qt::Alignment m_alignment = Qt::AlignLeft;
	double m_fontSize = 8.0;
	QString m_fontName = "Arial";
	QColor m_foregroundColor = Qt::black;
	QColor m_backgroundColor = Qt::transparent;
};


} // namespace imtreport


