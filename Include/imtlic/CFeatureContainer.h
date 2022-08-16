#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/IFeatureDependenciesManager.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlic
{

class CFeatureContainer: public imtbase::TAggergatedObjectCollectionWrap<imtlic::IFeaturePackage, CFeatureInfo>
{

public:
    typedef imtbase::TAggergatedObjectCollectionWrap<imtlic::IFeaturePackage, CFeatureInfo> BaseClass;

    CFeatureContainer();

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

    // reimplemented (IFeatureInfoProvider)
    virtual const imtbase::ICollectionInfo& GetFeatureList() const override;
    virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const override;
	virtual const imtbase::IObjectCollection* GetFeaturePackages() const override;

	// reimplemented (IFeatureDependenciesProvider)
	virtual const IFeatureDependenciesProvider* GetDependenciesInfoProvider() const override;
	virtual const imtbase::ICollectionInfo* GetParentFeatureInfoProviderList() const override;
	virtual const IFeatureInfoProvider* GetParentFeatureInfoProvider(const QByteArray& parentId) const override;

    // reimplemented (iser::ISerializable)
    virtual bool Serialize(iser::IArchive& archive) override;

    // reimplemented (istd::IChangeable)
    virtual int GetSupportedOperations() const override;
    virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
    virtual bool IsEqual(const IChangeable& object) const override;
    virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
    QByteArray m_packageId;
};


} // namespace imtlic


