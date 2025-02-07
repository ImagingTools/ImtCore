#pragma once


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Features.h>


namespace imtlicgql
{


class CFeatureCollectionControllerComp: public sdl::imtlic::Features::CFeatureCollectionControllerCompBase
{
public:
	typedef sdl::imtlic::Features::CFeatureCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureCollectionControllerComp);
		I_ASSIGN(m_featureInfoFactCompPtr, "FeatureFactory", "Factory used for creation of the new feature instance", true, "FeatureFactory");
	I_END_COMPONENT;

protected:
	bool CreateFeatureFromRepresentationModel(
				const sdl::imtlic::Features::CFeatureData::V1_0& featureRepresentationData,
				const QByteArray& rootFeatureId,
				imtlic::CFeatureInfo& featureInfo,
				QString& errorMessage) const;
	bool CreateRepresentationModelFromFeatureInfo(
				const imtlic::CFeatureInfo& featureInfo,
				sdl::imtlic::Features::CFeatureData::V1_0& featureRepresentationData,
				QString& errorMessage) const;

	// reimplemented (sdl::imtlic::Features::CFeatureCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtlic::Features::CFeaturesListGqlRequest& featuresListRequest,
				sdl::imtlic::Features::CFeatureItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Features::CFeatureData::V1_0& featureDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Features::CGetFeatureItemGqlRequest& featureItemRequest,
				sdl::imtlic::Features::CFeatureDataPayload::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtlic::Features::CUpdateFeatureGqlRequest& updateFeatureRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	I_FACT(imtlic::IFeatureInfo, m_featureInfoFactCompPtr);
};


} // namespace imtlicgql


