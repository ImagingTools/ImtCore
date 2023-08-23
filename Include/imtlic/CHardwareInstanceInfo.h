#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtlic/IHardwareInstanceInfo.h>


namespace imtlic
{


/**
	Common implementation of ILicensedHardwareInstanceInfo interface.
	\sa IHardwareInstanceInfo
	\ingroup LicenseManagement
*/
class CHardwareInstanceInfo: virtual public IHardwareInstanceInfo
{
public:
	CHardwareInstanceInfo();

	// reimplemented (imtlic::IHardwareInstanceInfo)
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QByteArray GetSoftwareId() const override;
	virtual void SetSoftwareId(const QByteArray& softwareId) override;
	virtual QByteArray GetModelTypeId() const override;
	virtual void SetModelTypeId(const QByteArray& typeId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplemented (istd::IChangeable)
	virtual QByteArray GetFactoryId() const override;

	QByteArray GetDeviceId(); // deprecated

private:
	QByteArray m_productId;
	QByteArray m_deviceId;
	QByteArray m_softwareId;
	QByteArray m_modelTypeId;
};

typedef imtbase::TIdentifiableWrap<CHardwareInstanceInfo> CIdentifiableHardwareInstanceInfo;


} // namespace imtlic


