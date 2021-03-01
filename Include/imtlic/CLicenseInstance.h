#pragma once


// ImtCore includes
#include <imtlic/ILicenseInstance.h>
#include <imtlic/CLicenseInfo.h>


namespace imtlic
{


/**
	Common implementation of the information about a single license.
	\sa ILicenseInfo
	\ingroup LicenseManagement
*/
class CLicenseInstance: virtual public imtlic::ILicenseInstance, public CLicenseInfo
{
public:
	typedef CLicenseInfo BaseClass;

	static QByteArray GetTypeId();

	// reimplemented (imtlic::ILicenseInstance)
	virtual QDateTime GetExpiration() const override;
	void SetExpiration(const QDateTime& expirationTime) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QDateTime m_expirationTime;
};


} // namespace imtlic


