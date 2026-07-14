#pragma once


#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


// custom types includes
#include <complextestsdl/SDL/1.0/CPP/ComplexUnion1_fwd.h>
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes_fwd.h>
#include <imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter_fwd.h>


namespace sdl::V1_0::complextest
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

class CComplexUnion2GqlHandlerCompBase: public ::imtservergql::CPermissibleGqlRequestHandlerComp
{

public:
	typedef ::imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CComplexUnion2GqlHandlerCompBase)
	I_END_COMPONENT

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual QJsonObject CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// abstract methods
	virtual V1_0::complextest::CProductOverview OnGetLastProductionResults(const CGetLastProductionResultsGqlRequest& getLastProductionResultsRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual V1_0::complextest::CCDMResult OnGetLastProductionResultsCDM(const CGetLastProductionResultsCDMGqlRequest& getLastProductionResultsCDMRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};


} // namespace sdl::V1_0::complextest
