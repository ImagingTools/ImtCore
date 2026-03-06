#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/IJwtSessionController.h>

// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ContactInfos.h>


namespace controlsgallerygql
{


class CContactInfoCollectionControllerComp: public sdl::controlsgallery::ContactInfos::CContactInfoCollectionControllerCompBase
{
public:
	typedef sdl::controlsgallery::ContactInfos::CContactInfoCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CContactInfoCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::controlsgallery::ContactInfos::CContactInfoCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::controlsgallery::ContactInfos::CContactInfoListGqlRequest& contactInfoListRequest,
				sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::controlsgallery::ContactInfos::CGetContactInfoGqlRequest& getContactInfoRequest,
				sdl::controlsgallery::ContactInfos::CContactInfoData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::controlsgallery::ContactInfos::CContactInfoData::V1_0& contactInfoDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::controlsgallery::ContactInfos::CUpdateContactInfoGqlRequest& updateContactInfoRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
private:
	bool FillObjectFromRepresentation(
				const sdl::controlsgallery::ContactInfos::CContactInfoData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
};


} // namespace controlsgallerygql


