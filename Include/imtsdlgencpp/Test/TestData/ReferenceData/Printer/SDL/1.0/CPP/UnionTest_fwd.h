#pragma once


#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::modsdl::UnionTest
{

// type forward declarations
class CCoords;
class CPrinterSpecificationBase;
class CLink;
class CPrinterBase;

// union forward declarations
class PrinterSpecification;
class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;

// request forward declarations
class CGetSpecificationsGqlRequest;

// generated base class forward declarations
class CPrinterCollectionControllerCompBase;

class CGraphQlHandlerCompBase: public ::imtservergql::CPermissibleGqlRequestHandlerComp
{

public:
	typedef ::imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CGraphQlHandlerCompBase)
	I_END_COMPONENT

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual QJsonObject CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// abstract methods
	virtual PrinterSpecification OnGetSpecifications(const CGetSpecificationsGqlRequest& getSpecificationsRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};



} // namespace sdl::modsdl::UnionTest
