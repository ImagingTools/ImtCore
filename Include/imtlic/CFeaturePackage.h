#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/IFeatureDependenciesManager.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlic
{


class CFeaturePackage:
			public imtbase::TAggergatedObjectCollectionWrap<IFeatureInfoProvider, CFeatureInfo>,
			virtual public IFeatureDependenciesManager
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IFeatureInfoProvider, CFeatureInfo> BaseClass;

	CFeaturePackage();

	void SetParents(const QByteArrayList& parentIds);

	// reimplemented (IFeatureDependenciesManager)
	void SetFeatureDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds);

	// reimplemented (IFeatureDependenciesProvider)
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const override;
	virtual const IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const override;

	// reimplemented (IFeatureInfoProvider)
	virtual const imtbase::IObjectCollection* GetFeaturePackages() const override;
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;
	virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const IFeatureDependenciesProvider* GetDependenciesInfoProvider() const override;
	virtual const imtbase::ICollectionInfo* GetParentFeatureInfoProviderList() const override;
	virtual const IFeatureInfoProvider* GetParentFeatureInfoProvider(const QByteArray& parentId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	const imtbase::IObjectCollection* m_featurePackageCollectionPtr;

private:
	QMap<QByteArray, QByteArrayList> m_dependencies;
	imtbase::CCollectionInfo m_parents;
};


} // namespace imtlic


