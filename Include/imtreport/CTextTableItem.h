#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <iser/ISerializable.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <icmm/CRgb.h>
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
				Qt::Alignment alignment = Qt::AlignLeft,
				CFont font = CFont("Arial", 10.0),
				const icmm::CRgb& foregroundColor = icmm::CRgb(0.0, 0.0, 0.0),
				const icmm::CRgb& backgroundColor = icmm::CRgb(1.0, 1.0, 1.0),
				const iimg::CBitmap& image = iimg::CBitmap());

	QString GetText() const;
	void SetText(const QString& text);

	Qt::Alignment GetAlignment() const;
	void SetAlignment(const Qt::Alignment alignment);

	const CFont& GetFont() const;
	void SetFont(const CFont& font);

	icmm::CRgb GetForegroundColor() const;
	void SetForegroundColor(const icmm::CRgb& color);

	icmm::CRgb GetBackgroundColor() const;
	void SetBackgroundColor(const icmm::CRgb& color);

	const iimg::CBitmap& GetImage() const;
	void SetImage(const iimg::CBitmap& image);

	bool operator==(const CTextTableItem &object) const ;
	bool operator!=(const CTextTableItem &object) const;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented istd::IChangeable
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

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
	icmm::CRgb m_foregroundColor;
	icmm::CRgb m_backgroundColor;
	iimg::CBitmap m_image;
};


} // namespace imtreport


