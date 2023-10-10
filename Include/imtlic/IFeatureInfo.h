#pragma once


// ACF includes
#include <istd/TPointerVector.h>
#include <istd/TDelPtr.h>
#include <iser/ISerializable.h>

//ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	Common information about a feature.
*/
class IFeatureInfo: virtual public iser::ISerializable
{
public:
	typedef QSet<QByteArray> FeatureIds;

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
		Get the feature ID.
	*/
	virtual bool IsOptional() const = 0;

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
	virtual const IFeatureInfo* GetSubFeature(const QByteArray& subfeatureId, int maxDepth = -1) const = 0;

	/**
		Get sub features for this feature.
	*/
	virtual const istd::TPointerVector<const IFeatureInfo>& GetSubFeatures() const  = 0;

	/**
		Insert sub feature.
	*/
	virtual bool InsertSubFeature(const IFeatureInfo* subFeatureInfo) = 0;

	/**
		Remove sub feature.
	*/
	virtual void DeleteSubFeature(const QByteArray& subFeatureId) = 0;

	/**
		Get list of features the given feature depends on.
	*/
	virtual QByteArrayList GetDependencies() const = 0;
};


typedef istd::TPointerVector<const IFeatureInfo> FeatureInfoList;


} // namespace imtlic


