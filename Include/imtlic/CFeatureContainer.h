#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlic
{


class CFeatureContainer: public imtbase::TAggergatedObjectCollectionWrap<IFeatureInfoProvider, CFeatureInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<IFeatureInfoProvider, CFeatureInfo> BaseClass;

	CFeatureContainer();

	/**
		Find feature object using feature-ID (not the collection of the feature)
	*/
	const IFeatureInfo* FindFeatureById(const QByteArray& featureId) const;

	/**
		Get collection-ID using feature-ID.
	*/
	QByteArray GetFeatureCollectionId(const QByteArray& featureId) const;

	// reimplemented (IFeatureInfoProvider)
	virtual const imtbase::ICollectionInfo& GetFeatureList() const override;
	virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const override;
};


} // namespace imtlic


