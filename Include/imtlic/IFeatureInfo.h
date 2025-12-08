#pragma once


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QVector>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/IObject.h>


namespace imtlic
{


/**
	Common information about a feature.
*/
class IFeatureInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_FEATURE_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_FEATURE_NAME,
		MIT_FEATURE_DESCRIPTION,
		MIT_IS_OPTIONAL,
		MIT_IS_PERMISSION
	};

	typedef QSet<QByteArray> FeatureIds;
	typedef istd::TSharedInterfacePtr<IFeatureInfo> FeatureInfoPtr;
	typedef QVector<FeatureInfoPtr> FeatureInfoList;

	/**
		Get the feature ID.
	*/
	virtual QByteArray GetFeatureId() const = 0;

	/**
		Get the feature name.
	*/
	virtual QString GetFeatureName() const = 0;

	/**
		Get the feature description.
	*/
	virtual QString GetFeatureDescription() const = 0;

	/**
		Feature is optional.
	*/
	virtual bool IsOptional() const = 0;

	/**
		Feature is permission.
	*/
	virtual bool IsPermission() const = 0;

	/**
		Get the parent feature pointer.
	*/
	virtual const IFeatureInfo* GetParentFeature() const = 0;

	/**
		Get IDs of subfeatures.
		\param maxDepth	traversation depth for tree iteration. negative value means unlimited depth.
	*/
	virtual QByteArrayList GetSubFeatureIds(int maxDepth = -1) const = 0;

	/**
		Get sub feature for this feature.
		\param maxDepth	traversation depth for tree iteration. negative value means unlimited depth.
	*/
	virtual FeatureInfoPtr GetSubFeature(const QByteArray& subfeatureId, int maxDepth = -1) const = 0;

	/**
		Get sub features for this feature.
	*/
	virtual const FeatureInfoList& GetSubFeatures() const  = 0;

	/**
		Insert sub feature.
	*/
	virtual bool InsertSubFeature(FeatureInfoPtr subFeatureInfo) = 0;

	/**
		Remove sub feature.
	*/
	virtual void DeleteSubFeature(const QByteArray& subFeatureId) = 0;

	/**
		Get list of features the given feature depends on.
	*/
	virtual QByteArrayList GetDependencies() const = 0;
};


typedef istd::TUniqueInterfacePtr<IFeatureInfo> IFeatureInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IFeatureInfo> IFeatureInfoSharedPtr;


} // namespace imtlic


