#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


/**
	Common interface for providing of features.
*/
class IFeatureInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of license infos given as QStringList.
		*/
		MIT_FEATURE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1,
	};

	/**
		Get feature info list.
	*/
	virtual const imtbase::ICollectionInfo& GetFeatureList() const = 0;

	/**
		Get feature info.
	*/
	virtual IFeatureInfoSharedPtr GetFeatureInfo(const QByteArray& featureId) const = 0;
};


} // namespace imtlic


