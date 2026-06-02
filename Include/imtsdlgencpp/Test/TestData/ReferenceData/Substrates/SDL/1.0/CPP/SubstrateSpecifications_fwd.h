#pragma once


#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


// custom types includes
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes_fwd.h>


namespace sdl::V1_0::V1_0
{

// type forward declarations
class CInputId;
class CCollectionViewParamsTest;
class CSubstrateSpecificationDocumentTypeIds;
class CSubstrateSpecificationListInput;
class CSubstrateSpecificationListItem;
class CSubstrateSpecificationListResponse;
class CCardboardSubstrateSpecification;
class CPaperSubstrateSpecification;
class CFilmSubstrateSpecification;
class CSubstrateSpecificationInput;
class CSubstrateSpecificationDocumentMetaInfo;

// union forward declarations
class SubstrateSpecificationDocument;
class CSubstrateSpecificationDocumentObject;
class CSubstrateSpecificationDocumentObjectList;

// request forward declarations
class CGetSubstrateSpecificationListGqlRequest;
class CGetSubstrateSpecificationGqlRequest;
class CGetOptionsListGqlRequest;
class CInsertSubstrateSpecificationGqlRequest;
class CUpdateSubstrateSpecificationGqlRequest;

class CSubstrateSpecificationCollectionControllerCompBase: public ::imtservergql::CObjectCollectionControllerCompBase
{

public:
	typedef ::imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSubstrateSpecificationCollectionControllerCompBase)
	I_END_COMPONENT

	virtual QMap<int, QByteArray> GetSupportedCommandIds() const override;

protected:
	// reimplemented (::imtservergql::CObjectCollectionControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const override;
	virtual ::imtservergql::CObjectCollectionControllerCompBase::GqlItemSetupContext CreateGqlItemSetupContext(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool SetupGqlItemWithContext(const ::imtgql::CGqlRequest& gqlRequest, const ::imtservergql::CObjectCollectionControllerCompBase::GqlItemSetupContext& setupContext, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const override;
	virtual bool UpdateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;
	virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// SubstrateSpecification methods
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const CGetSubstrateSpecificationListGqlRequest& getSubstrateSpecificationListRequest,
				CSubstrateSpecificationListItem& representationObject,
				QString& errorMessage) const = 0;
	virtual sdl::V1_0::imtbase::COptionsList  OnGetOptionsList(const CGetOptionsListGqlRequest& getOptionsListGqlRequest, QString& errorMessage) const = 0;

	// SubstrateSpecification methods
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const CGetSubstrateSpecificationGqlRequest& getSubstrateSpecificationRequest,
				SubstrateSpecificationDocument& representationPayload,
				QString& errorMessage) const = 0;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const SubstrateSpecificationDocument& substrateSpecificationDocumentRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const = 0;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const CUpdateSubstrateSpecificationGqlRequest& updateSubstrateSpecificationRequest,
				istd::IChangeable& object,
				QString& errorMessage) const = 0;

};

class CSubstrateSpecificationsGqlHandlerCompBase: public ::imtservergql::CPermissibleGqlRequestHandlerComp
{

public:
	typedef ::imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CSubstrateSpecificationsGqlHandlerCompBase)
	I_END_COMPONENT

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// abstract methods
	virtual CSubstrateSpecificationListResponse OnGetSubstrateSpecificationList(const CGetSubstrateSpecificationListGqlRequest& getSubstrateSpecificationListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual SubstrateSpecificationDocument OnGetSubstrateSpecification(const CGetSubstrateSpecificationGqlRequest& getSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual sdl::V1_0::imtbase::COptionsList OnGetOptionsList(const CGetOptionsListGqlRequest& getOptionsListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CInputId OnInsertSubstrateSpecification(const CInsertSubstrateSpecificationGqlRequest& insertSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CInputId OnUpdateSubstrateSpecification(const CUpdateSubstrateSpecificationGqlRequest& updateSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};



} // namespace sdl::V1_0::V1_0
