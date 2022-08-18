#pragma once


// ImtCore includes
#include <imtlic/CFeatureContainer.h>
#include <imtlic/IFeatureDependenciesManager.h>


namespace imtlic
{

class CFeaturePackage:
			public CFeatureContainer,
			virtual public IFeatureDependenciesManager
{

public:
	CFeaturePackage();

	void SetParents(const QByteArrayList& parentIds);

	// reimplemented (IFeatureInfoProvider)
	virtual const imtbase::IObjectCollection* GetFeaturePackages() const override;

	// reimplemented (IFeatureDependenciesManager)
	void SetFeatureDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds) override;

	// reimplemented (IFeatureDependenciesProvider)
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const override;
	virtual const IFeatureDependenciesProvider* GetDependenciesInfoProvider() const override;
	virtual const imtbase::ICollectionInfo* GetParentFeatureInfoProviderList() const override;
	virtual const IFeatureInfoProvider* GetParentFeatureInfoProvider(const QByteArray& parentId) const override;
	virtual const IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const override;

	// reimplemented (IObjectCollection)
	virtual bool RemoveElement(const Id& elementId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	void CleanupDependencies();

protected:
	const imtbase::IObjectCollection* m_featurePackageCollectionPtr;

private:
	QMap<QByteArray, QByteArrayList> m_dependencies;
	imtbase::CCollectionInfo m_parents;
};


} // namespace imtlic


