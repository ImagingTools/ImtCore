// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtlic/ILicenseDefinition.h>


namespace imtlic
{


/**
	Common implementation of the information about a single license.
	\sa ILicenseDefinition
	\ingroup LicenseManagement
*/
class CLicenseDefinition: virtual public imtlic::ILicenseDefinition
{
public:
	QByteArray GetTypeId() const;

	CLicenseDefinition();

	bool ContainsFeature(const QByteArray& featureId) const;
	bool RemoveFeature(const QByteArray& featureId);
	bool AddFeature(ILicenseDefinition::FeatureInfo& featureInfo);

	// reimplemented (imtlic::ILicenseDefinition)
	virtual QString GetLicenseName() const override;
	virtual void SetLicenseName(const QString& licenseName) override;
	virtual QByteArray GetLicenseId() const override;
	virtual void SetLicenseId(const QByteArray& licenseId) override;
	virtual QString GetLicenseDescription() const override;
	virtual void SetLicenseDescription(const QString& licenseDescription) override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QByteArrayList GetDependencies() const override;
	virtual void SetDependencies(QByteArrayList dependencies) override;
	virtual ILicenseDefinition::FeatureInfos GetFeatureInfos() const override;
	virtual void SetFeatureInfos(const ILicenseDefinition::FeatureInfos& featureInfos) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_licenseId;
	QByteArray m_productId;
	QString m_licenseName;
	QString m_description;
	QByteArrayList m_dependencies;
	FeatureInfos m_featureInfos;
};


typedef imtbase::TIdentifiableWrap<CLicenseDefinition> CIdentifiableLicenseDefinition;


} // namespace imtlic


