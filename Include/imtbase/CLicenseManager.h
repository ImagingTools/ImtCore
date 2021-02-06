#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtbase/ILicenseInfoProvider.h>
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


/**
	Common status manager implementation.
*/
class CLicenseManager: virtual public imtbase::ILicenseInfoProvider
{
public:
	CLicenseManager();

	// reimplemented (imtbase::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtbase::ILicenseInfo* GetLicenseInfo(const QByteArray & licenseId) const override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	CObjectCollection m_licenses;
};


} // namespace imtbase


