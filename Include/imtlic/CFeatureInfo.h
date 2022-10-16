#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfo.h>
//#include <imtlic/CFeatureContainer.h>


namespace imtlic
{


class CFeatureInfo: virtual public IFeatureInfo
{
public:
	CFeatureInfo();

	void SetFeatureId(const QByteArray& featureId);
	void SetFeatureName(const QString& featureName);
	void SetOptional(bool optional);
	void SetParentFeature(const IFeatureInfo* parentFeaturePtr);

	// reimplemented (IFeatureInfo)
	virtual const imtlic::IFeatureInfoProvider* GetFeaturePackage() const override;
	virtual QByteArray GetFeatureId() const override;
	virtual QString GetFeatureName() const override;
	virtual bool IsOptional() const override;
	virtual const IFeatureInfo* GetParentFeature() const override;
	virtual QList<const IFeatureInfo*> GetSubFeatures() const override;
	virtual bool InsertSubFeature(const IFeatureInfo* subFeatureInfo) override;
	virtual void DeleteSubFeature(const QByteArray &subFeatureId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	const imtlic::IFeatureInfoProvider* m_packagePtr;
	QByteArray m_id;
	QString m_name;
	bool m_optional;
	QList<const IFeatureInfo*> m_subFeatures;
	const IFeatureInfo* m_parentFeaturePtr;
};


} // namespace imtlic


