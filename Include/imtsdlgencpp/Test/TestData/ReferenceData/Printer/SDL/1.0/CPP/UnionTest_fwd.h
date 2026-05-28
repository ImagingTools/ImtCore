#pragma once


#include <imtservergql/CObjectCollectionControllerCompBase.h>
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

class CPrinterCollectionControllerCompBase: public ::imtservergql::CObjectCollectionControllerCompBase
{

public:
	typedef ::imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CPrinterCollectionControllerCompBase)
	I_END_COMPONENT

	virtual QMap<int, QByteArray> GetSupportedCommandIds() const override;

protected:
	// reimplemented (::imtservergql::CObjectCollectionControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const override;
	virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, QJsonObject& dataObj, QString& errorMessage) const override;

protected:
	// Printer methods
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const CGetSpecificationsGqlRequest& getSpecificationsRequest,
				PrinterSpecification& representationPayload,
				QString& errorMessage) const = 0;

};

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
