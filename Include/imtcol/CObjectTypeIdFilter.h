#pragma once


// ImtCore includes
#include <imtcol/IObjectTypeIdFilter.h>


namespace imtcol
{


class CObjectTypeIdFilter : virtual public IObjectTypeIdFilter
{
public:
	// reimplemented (imtcol::IObjectTypeIdFilter)
	QByteArray GetObjectTypeId() const override;
	void SetObjectTypeId(const QByteArray& typeId) override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_typeId;
};


} // namespace imtcol


