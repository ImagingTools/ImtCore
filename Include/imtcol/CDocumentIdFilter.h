#pragma once


// ImtCore includes
#include <imtcol/IDocumentIdFilter.h>


namespace imtcol
{


class CDocumentIdFilter : virtual public IDocumentIdFilter
{
public:
	CDocumentIdFilter();

	// reimplemented (imtcol::IDocumentIdFilter)
	virtual QByteArrayList GetDocumentIds() const override;
	virtual void SetDocumentIds(const QByteArrayList& documentIds) override;
	virtual ConditionType GetConditionType() const override;
	virtual void SetConditionType(ConditionType type) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArrayList m_documentIds;
	ConditionType m_conditionType;
};


} // namespace imtcol


