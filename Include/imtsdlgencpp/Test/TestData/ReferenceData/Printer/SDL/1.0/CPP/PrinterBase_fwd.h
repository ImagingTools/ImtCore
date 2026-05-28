#pragma once


#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::V1_0::modsdl
{

// type forward declarations
class CPrinterSpecificationBase;
class CLink;
class CPrinterBase;
class CPrinterList;

// union forward declarations
class PrinterSpecification;
class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;
class SimpleUnion;
class CSimpleUnionObject;
class CSimpleUnionObjectList;
class MixedUnion;
class CMixedUnionObject;
class CMixedUnionObjectList;

// request forward declarations
class CGetPrintersGqlRequest;

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
	virtual CPrinterList OnGetPrinters(const CGetPrintersGqlRequest& getPrintersRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};



} // namespace sdl::V1_0::modsdl
