#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QByteArray;
class QString;


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


class IFeatureInfo;


/**
	Common information about a feature group.
*/
class IFeatureInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get feature info list.
	*/
	virtual const imtbase::ICollectionInfo& GetFeatureList() const = 0;

	/**
		Get feature info.
	*/
	virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const = 0;
};


} // namespace imtlic


