#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtlic/CProductInstanceInfo.h>
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
	typedef imtlic::CProductInstanceInfo BaseClass;

	CHardwareInstanceInfo();

	// reimplemented (imtlic::IHardwareInstanceInfo)
	virtual QByteArray GetDeviceId() const override;
	virtual void SetDeviceId(const QByteArray& deviceId) override;
	virtual QByteArray GetSoftwareId() const override;
	virtual void SetSoftwareId(const QByteArray& softwareId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplemented (istd::IChangeable)
	virtual QByteArray GetFactoryId() const override;

private:
	QByteArray m_deviceId;
	QByteArray m_softwareId;
};

typedef imtbase::TIdentifiableWrap<CHardwareInstanceInfo> CIdentifiableWHardwareInstanceInfo;


} // namespace imtlic


