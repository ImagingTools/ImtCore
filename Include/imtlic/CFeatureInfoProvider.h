#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlic
{


class CFeatureInfoProvider:
			public imtbase::TAggergatedObjectCollectionWrap<IFeatureInfoProvider, CFeatureInfo>,
			virtual public IFeatureDependenciesProvider
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IFeatureInfoProvider, CFeatureInfo> BaseClass;

	CFeatureInfoProvider();

	void SetDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds);
	void SetParents(const QByteArrayList& parentIds);

	// reimplemented (IFeatureDependenciesProvider)
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const override;
	virtual const IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const override;

	// reimplemented (IFeatureInfoProvider)
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;
	virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const IFeatureDependenciesProvider* GetDependenciesInfoProvider() const override;
	virtual const imtbase::ICollectionInfo* GetParentFeatureInfoProviderList() const override;
	virtual const IFeatureInfoProvider* GetParentFeatureInfoProvider(const QByteArray& parentId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	imtbase::IObjectCollection* m_ownerPtr;

private:
	QMap<QByteArray, QByteArrayList> m_dependencies;
	imtbase::CCollectionInfo m_parentCollectionInfo;
};


} // namespace imtlic


