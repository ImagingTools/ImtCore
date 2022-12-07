#pragma once


// ACF includes
#include <iser/ISerializable.h>
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
		Get the feature package for this feature.
	*/
	virtual const imtlic::IFeatureInfoProvider* GetFeaturePackage() const = 0;

	/**
		Get the feature ID.
	*/
	virtual QByteArray GetFeatureId() const = 0;

	/**
		Get the feature name.
	*/
	virtual QString GetFeatureName() const = 0;

	/**
		Get the feature ID.
	*/
	virtual bool IsOptional() const = 0;

	/**
		Get the parent feature pointer.
	*/
	virtual const IFeatureInfo* GetParentFeature() const = 0;

	/**
		Get all sub features for this feature.
	*/
	virtual QVector<const IFeatureInfo*> GetSubFeatures() const  = 0;

	/**
		Insert sub feature.
	*/
	virtual bool InsertSubFeature(const IFeatureInfo* subFeatureInfo) = 0;

	/**
		Remove sub feature.
	*/
	virtual void DeleteSubFeature(const QByteArray& subFeatureId) = 0;
};


typedef QVector<const IFeatureInfo*> FeatureInfoList;


} // namespace imtlic


