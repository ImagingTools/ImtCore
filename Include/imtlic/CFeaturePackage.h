#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/IFeatureDependenciesManager.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlic
{


class CFeaturePackage:
			public imtbase::TAggergatedObjectCollectionWrap<IFeaturePackage, CFeatureInfo>,
			virtual public IFeatureDependenciesManager
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IFeaturePackage, CFeatureInfo> BaseClass;

	CFeaturePackage();

	void SetParents(const QByteArrayList& parentIds);

	/**
		Find feature object using feature-ID (not the collection of the feature)
	*/
	const IFeatureInfo* FindFeatureById(const QByteArray& featureId) const;

	/**
		Get collection-ID using feature-ID.
	*/
	QByteArray GetFeatureCollectionId(const QByteArray& featureId) const;

	// reimplemented (IFeaturePackage)
	virtual QByteArray GetPackageId() const override;
	virtual void SetPackageId(const QByteArray& packageId) override;

	// reimplemented (IFeatureDependenciesManager)
	void SetFeatureDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds) override;

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

	// reimplemented (IObjectCollection)
	virtual bool RemoveObject(const QByteArray& objectId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
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
	QByteArray m_packageId;
};


} // namespace imtlic


