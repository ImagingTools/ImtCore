#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/TIdentifiableWrap.h>
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


class CFeatureInfo: virtual public IFeatureInfo
{
public:
	CFeatureInfo();

	void SetFeatureId(const QByteArray& featureId);
	void SetFeatureName(const QString& featureName);
	void SetFeatureDescription(const QString& description);
	void SetOptional(bool optional);
	void SetParentFeature(const IFeatureInfo* parentFeaturePtr);
	void SetDependencies(QByteArrayList dependencies);

	// reimplemented (IFeatureInfo)
	virtual QByteArray GetFeatureId() const override;
	virtual QString GetFeatureName() const override;
	virtual QString GetFeatureDescription() const override;
	virtual bool IsOptional() const override;
	virtual const IFeatureInfo* GetParentFeature() const override;
	virtual QByteArrayList GetSubFeatureIds(int maxDepth = -1) const override;
	virtual const IFeatureInfo* GetSubFeature(const QByteArray& subfeatureId, int maxDepth = -1) const override;
	virtual const istd::TPointerVector<const IFeatureInfo>& GetSubFeatures() const override;
	virtual bool InsertSubFeature(const IFeatureInfo* subFeatureInfo) override;
	virtual void DeleteSubFeature(const QByteArray &subFeatureId) override;
	virtual QByteArrayList GetDependencies() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	void GetSubFeaturesRecursive(const FeatureInfoList& subFeatures, int maxDepth, int currentDepth, QByteArrayList& featureList) const;
	const IFeatureInfo* GetSubFeatureRecursive(const FeatureInfoList& subFeatures, const QByteArray& subfeatureId, int maxDepth, int currentDepth = 1) const;

protected:
	QByteArray m_id;
	QString m_name;
	QString m_description;
	bool m_optional;
	QByteArrayList m_dependencies;
	FeatureInfoList m_subFeatures;
	const IFeatureInfo* m_parentFeaturePtr;
};


typedef imtbase::TIdentifiableWrap<CFeatureInfo> CIdentifiableFeatureInfo;


} // namespace imtlic


