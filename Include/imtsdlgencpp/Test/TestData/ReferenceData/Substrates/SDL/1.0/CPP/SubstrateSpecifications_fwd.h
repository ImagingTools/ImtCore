#pragma once


#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::substrate::SubstrateSpecifications
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

// generated base class forward declarations
class CSubstrateSpecificationCollectionControllerCompBase;

class CGraphQlHandlerCompBase: public ::imtservergql::CPermissibleGqlRequestHandlerComp
{

public:
	typedef ::imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CGraphQlHandlerCompBase)
	I_END_COMPONENT

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// abstract methods
	virtual CSubstrateSpecificationListResponse OnGetSubstrateSpecificationList(const CGetSubstrateSpecificationListGqlRequest& getSubstrateSpecificationListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual SubstrateSpecificationDocument OnGetSubstrateSpecification(const CGetSubstrateSpecificationGqlRequest& getSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual imtbase::ImtBaseTypes::COptionsList OnGetOptionsList(const CGetOptionsListGqlRequest& getOptionsListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CInputId OnInsertSubstrateSpecification(const CInsertSubstrateSpecificationGqlRequest& insertSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CInputId OnUpdateSubstrateSpecification(const CUpdateSubstrateSpecificationGqlRequest& updateSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};



} // namespace sdl::substrate::SubstrateSpecifications
