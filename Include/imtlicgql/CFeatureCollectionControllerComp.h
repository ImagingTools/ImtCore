#pragma once


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <GeneratedFiles/imtlicsdl/SDL/CPP/Features/Features.h>


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
	// reimplemented (sdl::imtlic::Features::CFeatureCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtlic::Features::CGetFeaturesListGqlRequest& featuresListRequest,
				sdl::imtlic::Features::CFeatureItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtlic::Features::CFeatureData& featureDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtlic::Features::CGetFeatureItemGqlRequest& featureItemRequest,
				sdl::imtlic::Features::CFeatureDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				imtbase::CTreeItemModel& model,
				int itemIndex,
				const imtbase::IObjectCollectionIterator* objectCollectionIterator,
				QString& errorMessage) const override;

	virtual bool CreateChildModelRepresentation(const imtlic::CFeatureInfo& featureInfo, imtbase::CTreeItemModel& representationModel) const;

private:
	I_FACT(imtlic::IFeatureInfo, m_featureInfoFactCompPtr);
};


} // namespace imtlicgql


