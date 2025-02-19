#pragma once


// ImtCore includes
#include <imtcol/ITextFilter.h>


namespace imtcol
{


class CTextFilter : virtual public ITextFilter
{
public:
	// reimplemented (imtcol::ITextFilter)
	QByteArrayList GetFilteringFieldIds() const override;
	void SetFilteringFieldIds(const QByteArrayList& fieldIds) override;
	QString GetFilterText() const override;
	void SetFilterText(const QString& text) override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArrayList m_fieldIds;
	QString m_text;
};


} // namespace imtcol


