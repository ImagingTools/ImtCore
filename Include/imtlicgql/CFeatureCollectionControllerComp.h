#pragma once


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Features.h>


namespace imtlicgql
{


class CFeatureCollectionControllerComp: public sdl::imtlic::Features::V1_0::CFeatureCollectionControllerCompBase
{
public:
	typedef sdl::imtlic::Features::V1_0::CFeatureCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureCollectionControllerComp);
		I_ASSIGN(m_featureInfoFactCompPtr, "FeatureFactory", "Factory used for creation of the new feature instance", true, "FeatureFactory");
	I_END_COMPONENT;

protected:
	bool CreateFeatureFromRepresentationModel(const sdl::imtlic::Features::V1_0::CFeatureData& featureRepresentationData, imtlic::CFeatureInfo& featureInfo, QString& errorMessage) const;
	bool CreateRepresentationModelFromFeatureInfo(const imtlic::CFeatureInfo& featureInfo, sdl::imtlic::Features::V1_0::CFeatureData& featureRepresentationData, QString& errorMessage) const;

	// reimplemented (sdl::imtlic::Features::V1_0::CFeatureCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtlic::Features::V1_0::CFeaturesListGqlRequest& featuresListRequest,
				sdl::imtlic::Features::V1_0::CFeatureItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Features::V1_0::CFeatureData& featureDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Features::V1_0::CGetFeatureItemGqlRequest& featureItemRequest,
				sdl::imtlic::Features::V1_0::CFeatureDataPayload& representationPayload,
				QString& errorMessage) const override;

private:
	I_FACT(imtlic::IFeatureInfo, m_featureInfoFactCompPtr);
};


} // namespace imtlicgql


