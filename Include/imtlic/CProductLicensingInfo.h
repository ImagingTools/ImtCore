// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/ILicenseDependenciesManager.h>
#include <imtlic/CLicenseInfoManager.h>


namespace imtlic
{


/**
	Common implementation of IProductLicensingInfo interface.
	\ingroup LicenseManagement
*/
class CProductLicensingInfo:
			virtual public imtlic::IProductLicensingInfo,
			virtual public imtlic::ILicenseDependenciesManager,
			public CLicenseInfoManager
{
public:
	typedef CLicenseInfoManager BaseClass;

	CProductLicensingInfo();

	// reimplemented (imtlic::ILicenseDependenciesManager)
	virtual void SetLicenseDependencies(const QByteArray& licenseId, const QByteArrayList& dependentIds) override;

	// reimplemented (imtlic::ILicenseDependenciesProvider)
	virtual QByteArrayList GetLicenseDependencies(const QByteArray& licenseId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	imtbase::IObjectCollection* m_featurePackagesPtr;

	QString m_productName;
	QByteArray m_productId;
	QByteArray m_categoryId;

private:
	QMap<QByteArray, QByteArrayList> m_dependencies;
};


} // namespace imtlic


