#pragma once


#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::complextest::ComplexUnion2
{

// type forward declarations
class CResult;
class CArea;
class CAnalyzer;
class CInspection;
class CProductOverview;
class CLocalizedText;
class CCoordinates;
class CExtendedMetaData;
class CResultMetaData;
class COverallResultValues;
class CUnionTestingType;
class CCDMResultVarRecursive;
class CCDMResultVarString;
class CCDMResult;
class CPointsInCoordinateFormat;
class CCollectionViewParamsTest;

// union forward declarations
class ExtendedMetaDataUnionType;
class CExtendedMetaDataUnionTypeObject;
class CExtendedMetaDataUnionTypeObjectList;
class ExtendedMetaDataUnionSimpleType;
class CExtendedMetaDataUnionSimpleTypeObject;
class CExtendedMetaDataUnionSimpleTypeObjectList;
class CDMResultUnionType;
class CCDMResultUnionTypeObject;
class CCDMResultUnionTypeObjectList;

// request forward declarations
class CGetLastProductionResultsGqlRequest;
class CGetLastProductionResultsCDMGqlRequest;

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
	virtual CProductOverview OnGetLastProductionResults(const CGetLastProductionResultsGqlRequest& getLastProductionResultsRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CCDMResult OnGetLastProductionResultsCDM(const CGetLastProductionResultsCDMGqlRequest& getLastProductionResultsCDMRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};



} // namespace sdl::complextest::ComplexUnion2
