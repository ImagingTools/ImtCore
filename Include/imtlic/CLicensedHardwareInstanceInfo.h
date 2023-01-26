#pragma once


// ImtCore includes
#include <imtlic/CProductInstanceInfo.h>
#include <imtlic/ILicensedHardwareInstanceInfo.h>


namespace imtlic
{


/**
	Common implementation of ILicensedHardwareInstanceInfo interface.
	\sa ILicensedHardwareInstanceInfo
	\ingroup LicenseManagement
*/
class CLicensedHardwareInstanceInfo: public imtlic::CProductInstanceInfo, virtual public ILicensedHardwareInstanceInfo
{
public:
	typedef imtlic::CProductInstanceInfo BaseClass;

	CLicensedHardwareInstanceInfo();

	// reimplemented (imtlic::ILicensedHardwareInstanceInfo)
	virtual QByteArray GetSerialNumber() const override;
	virtual void SetSerialNumber(const QByteArray& serialNumber) override;
	virtual QByteArray GetSoftwareId() const override;
	virtual void SetSoftwareId(const QByteArray& softwareId) override;

	// reimplemented (imtlic::ILicenseInfoProvider)
	virtual QByteArray GetStatus() const override;
	virtual void SetStatus(const QByteArray& status) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
//	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplemented (istd::IChangeable)
	virtual QByteArray GetFactoryId() const override;

private:
	QByteArray m_serialNumber;
	QByteArray m_softwareId;
	QByteArray m_partStatus;
};


} // namespace imtlic


