#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/IJwtSessionController.h>

// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ContactInfos_fwd.h>


namespace controlsgallerygql
{


class CContactInfoCollectionControllerComp: public sdl::V1_0::controlsgallery::CContactInfoCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::controlsgallery::CContactInfoCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CContactInfoCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CVisualStatus OnGetObjectVisualStatus(
				const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::V1_0::controlsgallery::CContactInfoCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::controlsgallery::CContactInfoListGqlRequest& contactInfoListRequest,
				sdl::V1_0::controlsgallery::CContactInfoItemData& representationObject,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::controlsgallery::CGetContactInfoGqlRequest& getContactInfoRequest,
				sdl::V1_0::controlsgallery::CContactInfoData& representationPayload,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::controlsgallery::CContactInfoData& contactInfoDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::controlsgallery::CUpdateContactInfoGqlRequest& updateContactInfoRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::controlsgallery::CContactInfoData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
};


} // namespace controlsgallerygql


