#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


class QByteArray;
class QString;

/**
	Common information about a feature group.
*/
class IFeatureInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get human-readable name of the feature group.
	*/
	virtual QString GetFeatureGroupName() const = 0;

	/**
		Get feature info list.
	*/
	virtual const imtbase::ICollectionInfo& GetFeatureList() const = 0;

	/**
		Get feature info.
	*/
	virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& id) const = 0;

	/**
		Get parent feature group list.
	*/
	virtual const imtbase::ICollectionInfo& GetParentFeatureGroupList() const = 0;

	/**
		Get parent feature group.
	*/
	virtual const IFeatureInfoProvider* GetParentFeatureGroup(const QByteArray& id) const = 0;

	/**
		Get enabled feature list for given parent feature group ID.
	*/
	virtual QByteArrayList GetEnabledFeatureList(const QByteArray& id) const = 0;
};


} // namespace imtlic


