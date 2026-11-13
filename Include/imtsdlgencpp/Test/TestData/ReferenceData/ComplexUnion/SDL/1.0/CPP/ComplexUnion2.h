#pragma once

#include "ComplexUnion2_fwd.h"

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QMetaEnum>

// ACF includes
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtsdl/TElementList.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>

// custom types includes
#include <complextestsdl/SDL/1.0/CPP/ComplexUnion1.h>
#include <imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace sdl::V1_0::complextest
{


inline namespace ComplexUnion2SdlEnums
{



Q_NAMESPACE


} // inline namespace ComplexUnion2SdlEnums

// forward declaration for union types
class ExtendedMetaDataUnionType;
class ExtendedMetaDataUnionSimpleType;
class CDMResultUnionType;

class CResult
{
public:
	struct ResultFields
	{
		static const inline QString AreaResult = "AreaResult";
		static const inline QString ExpectedMinValue = "ExpectedMinValue";
		static const inline QString ExpectedMaxValue = "ExpectedMaxValue";
		static const inline QString MeasuredValue = "MeasuredValue";
		static const inline QString MinMeasuredValue = "MinMeasuredValue";
		static const inline QString MaxMeasuredValue = "MaxMeasuredValue";
		static const inline QString MeanMeasuredValue = "MeanMeasuredValue";
		static const inline QString MeasurementType = "MeasurementType";
		static const inline QString MeasurementUnit = "MeasurementUnit";
		static const inline QString Length = "Length";
		static const inline QString ErrorType = "ErrorType";
		static const inline QString Geometry = "Geometry";
	};

	istd::TNullableValue<StatusCode> AreaResult;
	istd::TNullableValue<double> ExpectedMinValue;
	istd::TNullableValue<double> ExpectedMaxValue;
	istd::TNullableValue<double> MeasuredValue;
	istd::TNullableValue<double> MinMeasuredValue;
	istd::TNullableValue<double> MaxMeasuredValue;
	istd::TNullableValue<double> MeanMeasuredValue;
	istd::TNullableValue<MeasurementType> MeasurementType;
	istd::TNullableValue<MeasurementUnit> MeasurementUnit;
	istd::TNullableValue<double> Length;
	istd::TNullableValue<ErrorCode> ErrorType;
	istd::TNullableValue<CGeometry> Geometry;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CResult& other) const;
	[[nodiscard]] bool operator!=(const CResult& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CArea
{
public:
	struct AreaFields
	{
		static const inline QString AreaName = "AreaName";
		static const inline QString Status = "Status";
		static const inline QString IconPosition = "IconPosition";
		static const inline QString ErrorType = "ErrorType";
		static const inline QString Results = "Results";
	};

	istd::TNullableValue<QString> AreaName;
	istd::TNullableValue<StatusCode> Status;
	istd::TNullableValue<CPoint> IconPosition;
	istd::TNullableValue<ErrorCode> ErrorType;
	istd::TNullableValue<imtsdl::TElementList<CResult>> Results;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CArea& other) const;
	[[nodiscard]] bool operator!=(const CArea& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CAnalyzer
{
public:
	struct AnalyzerFields
	{
		static const inline QString AnalyzerName = "AnalyzerName";
		static const inline QString AnalyzerResult = "AnalyzerResult";
		static const inline QString Areas = "Areas";
	};

	istd::TNullableValue<QString> AnalyzerName;
	istd::TNullableValue<StatusCode> AnalyzerResult;
	istd::TNullableValue<imtsdl::TElementList<CArea>> Areas;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CAnalyzer& other) const;
	[[nodiscard]] bool operator!=(const CAnalyzer& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CInspection
{
public:
	struct InspectionFields
	{
		static const inline QString ID = "ID";
		static const inline QString TypeID = "TypeID";
		static const inline QString Name = "Name";
		static const inline QString Status = "Status";
		static const inline QString Analyzers = "Analyzers";
	};

	istd::TNullableValue<QString> ID;
	istd::TNullableValue<QString> TypeID;
	istd::TNullableValue<QString> Name;
	istd::TNullableValue<StatusCode> Status;
	istd::TNullableValue<imtsdl::TElementList<CAnalyzer>> Analyzers;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CInspection& other) const;
	[[nodiscard]] bool operator!=(const CInspection& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CProductOverview
{
public:
	struct ProductOverviewFields
	{
		static const inline QString ProductID = "ProductID";
		static const inline QString Name = "Name";
		static const inline QString Status = "Status";
		static const inline QString PartSerialNumber = "PartSerialNumber";
		static const inline QString Timestamp = "Timestamp";
		static const inline QString ProductImage = "ProductImage";
		static const inline QString Inspections = "Inspections";
	};

	istd::TNullableValue<int> ProductID;
	istd::TNullableValue<QString> Name;
	istd::TNullableValue<StatusCode> Status;
	istd::TNullableValue<QString> PartSerialNumber;
	istd::TNullableValue<QString> Timestamp;
	istd::TNullableValue<QString> ProductImage;
	istd::TNullableValue<imtsdl::TElementList<CInspection>> Inspections;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CProductOverview& other) const;
	[[nodiscard]] bool operator!=(const CProductOverview& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CLocalizedText
{
public:
	struct LocalizedTextFields
	{
		static const inline QString Text = "text";
		static const inline QString Locale = "locale";
	};

	istd::TNullableValue<QString> text;
	istd::TNullableValue<QString> locale;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CLocalizedText& other) const;
	[[nodiscard]] bool operator!=(const CLocalizedText& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CCoordinates
{
public:
	struct CoordinatesFields
	{
		static const inline QString X = "x";
		static const inline QString Y = "y";
		static const inline QString Z = "z";
	};

	istd::TNullableValue<double> x;
	istd::TNullableValue<double> y;
	istd::TNullableValue<double> z;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CCoordinates& other) const;
	[[nodiscard]] bool operator!=(const CCoordinates& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CExtendedMetaData
{
public:
	struct ExtendedMetaDataFields
	{
		static const inline QString Key = "key";
		static const inline QString Value = "value";
	};

	istd::TNullableValue<QString> key;
	istd::TNullableValue<ExtendedMetaDataUnionType> value;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CExtendedMetaData& other) const;
	[[nodiscard]] bool operator!=(const CExtendedMetaData& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CResultMetaData
{
public:
	struct ResultMetaDataFields
	{
		static const inline QString ResultId = "resultId";
		static const inline QString CreationTime = "creationTime";
		static const inline QString PartId = "partId";
		static const inline QString ProductId = "productId";
		static const inline QString Name = "name";
		static const inline QString Classification = "classification";
		static const inline QString Description = "description";
		static const inline QString ExtendedMetaData = "extendedMetaData";
	};

	istd::TNullableValue<QByteArray> resultId;
	istd::TNullableValue<QString> creationTime;
	istd::TNullableValue<QString> partId;
	istd::TNullableValue<QString> productId;
	istd::TNullableValue<QString> name;
	istd::TNullableValue<int> classification;
	istd::TNullableValue<CLocalizedText> description;
	istd::TNullableValue<imtsdl::TElementList<CExtendedMetaData>> extendedMetaData;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CResultMetaData& other) const;
	[[nodiscard]] bool operator!=(const CResultMetaData& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class COverallResultValues
{
public:
	struct OverallResultValuesFields
	{
		static const inline QString Name = "name";
		static const inline QString ValueId = "valueId";
		static const inline QString MeasuredValue = "measuredValue";
		static const inline QString LowLimit = "lowLimit";
		static const inline QString HighLimit = "highLimit";
	};

	istd::TNullableValue<QString> name;
	istd::TNullableValue<QString> valueId;
	istd::TNullableValue<double> measuredValue;
	istd::TNullableValue<double> lowLimit;
	istd::TNullableValue<double> highLimit;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const COverallResultValues& other) const;
	[[nodiscard]] bool operator!=(const COverallResultValues& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CUnionTestingType
{
public:
	struct UnionTestingTypeFields
	{
		static const inline QString SimpleUnion = "simpleUnion";
		static const inline QString ComplexUnion = "complexUnion";
		static const inline QString MixedUnion = "mixedUnion";
	};

	istd::TNullableValue<ExtendedMetaDataUnionSimpleType> simpleUnion;
	istd::TNullableValue<CDMResultUnionType> complexUnion;
	istd::TNullableValue<ExtendedMetaDataUnionType> mixedUnion;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CUnionTestingType& other) const;
	[[nodiscard]] bool operator!=(const CUnionTestingType& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CCDMResultVarRecursive
{
public:
	struct CDMResultVarRecursiveFields
	{
		static const inline QString DataModelTitle = "dataModelTitle";
		static const inline QString DataModelVersion = "dataModelVersion";
		static const inline QString ResultMetaData = "resultMetaData";
		static const inline QString ResultContent = "resultContent";
		static const inline QString OverallResultValues = "overallResultValues";
	};

	istd::TNullableValue<QString> dataModelTitle;
	istd::TNullableValue<QString> dataModelVersion;
	istd::TNullableValue<CResultMetaData> resultMetaData;
	istd::TNullableValue<imtsdl::TElementList<CDMResultUnionType>> resultContent;
	istd::TNullableValue<imtsdl::TElementList<COverallResultValues>> overallResultValues;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CCDMResultVarRecursive& other) const;
	[[nodiscard]] bool operator!=(const CCDMResultVarRecursive& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CCDMResultVarString
{
public:
	struct CDMResultVarStringFields
	{
		static const inline QString DataModelTitle = "dataModelTitle";
		static const inline QString DataModelVersion = "dataModelVersion";
		static const inline QString ResultMetaData = "resultMetaData";
		static const inline QString ResultContent = "resultContent";
		static const inline QString OverallResultValues = "overallResultValues";
	};

	istd::TNullableValue<QString> dataModelTitle;
	istd::TNullableValue<QString> dataModelVersion;
	istd::TNullableValue<CResultMetaData> resultMetaData;
	istd::TNullableValue<imtsdl::TElementList<QString>> resultContent;
	istd::TNullableValue<imtsdl::TElementList<COverallResultValues>> overallResultValues;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CCDMResultVarString& other) const;
	[[nodiscard]] bool operator!=(const CCDMResultVarString& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CCDMResult
{
public:
	struct CDMResultFields
	{
		static const inline QString DataModelTitle = "dataModelTitle";
		static const inline QString DataModelVersion = "dataModelVersion";
		static const inline QString ResultMetaData = "resultMetaData";
		static const inline QString ResultContent = "resultContent";
		static const inline QString OverallResultValues = "overallResultValues";
	};

	istd::TNullableValue<QString> dataModelTitle;
	istd::TNullableValue<QString> dataModelVersion;
	istd::TNullableValue<CResultMetaData> resultMetaData;
	istd::TNullableValue<imtsdl::TElementList<CDMResultUnionType>> resultContent;
	istd::TNullableValue<imtsdl::TElementList<COverallResultValues>> overallResultValues;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CCDMResult& other) const;
	[[nodiscard]] bool operator!=(const CCDMResult& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CPointsInCoordinateFormat
{
public:
	struct PointsInCoordinateFormatFields
	{
		static const inline QString PointAsCoordinates = "pointAsCoordinates";
	};

	istd::TNullableValue<bool> pointAsCoordinates;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CPointsInCoordinateFormat& other) const;
	[[nodiscard]] bool operator!=(const CPointsInCoordinateFormat& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CCollectionViewParamsTest
{
public:
	struct CollectionViewParamsTestFields
	{
		static const inline QString Count = "count";
		static const inline QString Offset = "offset";
		static const inline QString FilterModel = "filterModel";
	};

	istd::TNullableValue<int> count;
	istd::TNullableValue<int> offset;
	istd::TNullableValue<imtbase::CComplexCollectionFilter> filterModel;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CCollectionViewParamsTest& other) const;
	[[nodiscard]] bool operator!=(const CCollectionViewParamsTest& other) const {return !(operator==(other));}
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};



class ExtendedMetaDataUnionType: public std::variant<CCoordinates, double, bool, QString> {

public:

	typedef std::variant<CCoordinates, double, bool, QString> BaseClass;

	ExtendedMetaDataUnionType(){};
	ExtendedMetaDataUnionType(const CCoordinates& ref)
		: BaseClass(ref){};

	ExtendedMetaDataUnionType(const double& ref)
		: BaseClass(ref){};

	ExtendedMetaDataUnionType(const bool& ref)
		: BaseClass(ref){};

	ExtendedMetaDataUnionType(const QString& ref)
		: BaseClass(ref){};

};



class ExtendedMetaDataUnionSimpleType: public std::variant<double, bool, QString> {

public:

	typedef std::variant<double, bool, QString> BaseClass;

	ExtendedMetaDataUnionSimpleType(){};
	ExtendedMetaDataUnionSimpleType(const double& ref)
		: BaseClass(ref){};

	ExtendedMetaDataUnionSimpleType(const bool& ref)
		: BaseClass(ref){};

	ExtendedMetaDataUnionSimpleType(const QString& ref)
		: BaseClass(ref){};

};



class CDMResultUnionType: public std::variant<CCDMResultVarString, CCDMResultVarRecursive> {

public:

	typedef std::variant<CCDMResultVarString, CCDMResultVarRecursive> BaseClass;

	CDMResultUnionType(){};
	CDMResultUnionType(const CCDMResultVarString& ref)
		: BaseClass(ref){};

	CDMResultUnionType(const CCDMResultVarRecursive& ref)
		: BaseClass(ref){};

};



struct GetLastProductionResultsRequestInfo
{
	bool isProductIDRequested = true;
	bool isNameRequested = true;
	bool isStatusRequested = true;
	bool isPartSerialNumberRequested = true;
	bool isTimestampRequested = true;
	bool isProductImageRequested = true;
	bool isInspectionsRequested = true;
		struct InspectionsRequestInfo
		{
			bool isIDRequested = true;
			bool isTypeIDRequested = true;
			bool isNameRequested = true;
			bool isStatusRequested = true;
			bool isAnalyzersRequested = true;
			struct AnalyzersRequestInfo
			{
				bool isAnalyzerNameRequested = true;
				bool isAnalyzerResultRequested = true;
				bool isAreasRequested = true;
				struct AreasRequestInfo
				{
					bool isAreaNameRequested = true;
					bool isStatusRequested = true;
					bool isIconPositionRequested = true;
					bool isErrorTypeRequested = true;
					bool isResultsRequested = true;
					struct IconPositionRequestInfo
					{
						bool isXRequested = true;
						bool isYRequested = true;
					} IconPosition;
					struct ResultsRequestInfo
					{
						bool isAreaResultRequested = true;
						bool isExpectedMinValueRequested = true;
						bool isExpectedMaxValueRequested = true;
						bool isMeasuredValueRequested = true;
						bool isMinMeasuredValueRequested = true;
						bool isMaxMeasuredValueRequested = true;
						bool isMeanMeasuredValueRequested = true;
						bool isMeasurementTypeRequested = true;
						bool isMeasurementUnitRequested = true;
						bool isLengthRequested = true;
						bool isErrorTypeRequested = true;
						bool isGeometryRequested = true;
						struct GeometryRequestInfo
						{
							bool isGeometryTypeRequested = true;
							bool isRadiusRequested = true;
							bool isPointsRequested = true;
							bool isRequiredPointsRequested = true;
							bool isOptionalPointsRequested = true;
							struct PointsRequestInfo
							{
								bool isXRequested = true;
								bool isYRequested = true;
							} Points;
							struct RequiredPointsRequestInfo
							{
								bool isXRequested = true;
								bool isYRequested = true;
							} RequiredPoints;
							struct OptionalPointsRequestInfo
							{
								bool isXRequested = true;
								bool isYRequested = true;
							} OptionalPoints;
						} Geometry;
					} Results;
				} Areas;
			} Analyzers;
		} Inspections;
};


struct GetLastProductionResultsRequestArguments
{
};


class CGetLastProductionResultsGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetLastProductionResultsRequestArguments& requestArguments, const GetLastProductionResultsRequestInfo& requestInfo = GetLastProductionResultsRequestInfo());

	CGetLastProductionResultsGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetLastProductionResultsRequestArguments& GetRequestedArguments() const;
	GetLastProductionResultsRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetLastProductionResultsRequestArguments m_requestedArguments;
	GetLastProductionResultsRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};




struct GetLastProductionResultsCDMRequestInfo
{
	bool isDataModelTitleRequested = true;
	bool isDataModelVersionRequested = true;
	bool isResultMetaDataRequested = true;
	bool isResultContentRequested = true;
	bool isOverallResultValuesRequested = true;
		struct resultMetaDataRequestInfo
		{
			bool isResultIdRequested = true;
			bool isCreationTimeRequested = true;
			bool isPartIdRequested = true;
			bool isProductIdRequested = true;
			bool isNameRequested = true;
			bool isClassificationRequested = true;
			bool isDescriptionRequested = true;
			bool isExtendedMetaDataRequested = true;
			struct descriptionRequestInfo
			{
				bool isTextRequested = true;
				bool isLocaleRequested = true;
			} description;
			struct extendedMetaDataRequestInfo
			{
				bool isKeyRequested = true;
				bool isValueRequested = true;
			} extendedMetaData;
		} resultMetaData;
		struct overallResultValuesRequestInfo
		{
			bool isNameRequested = true;
			bool isValueIdRequested = true;
			bool isMeasuredValueRequested = true;
			bool isLowLimitRequested = true;
			bool isHighLimitRequested = true;
		} overallResultValues;
};


struct GetLastProductionResultsCDMRequestArguments
{
	istd::TNullableValue<V1_0::complextest::CPointsInCoordinateFormat> pointAsCoordinates;
};


class CGetLastProductionResultsCDMGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetLastProductionResultsCDMRequestArguments& requestArguments, const GetLastProductionResultsCDMRequestInfo& requestInfo = GetLastProductionResultsCDMRequestInfo());

	CGetLastProductionResultsCDMGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetLastProductionResultsCDMRequestArguments& GetRequestedArguments() const;
	GetLastProductionResultsCDMRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetLastProductionResultsCDMRequestArguments m_requestedArguments;
	GetLastProductionResultsCDMRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};


} // namespace sdl::V1_0::complextest


Q_DECLARE_METATYPE(sdl::V1_0::complextest::CResult);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CArea);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CAnalyzer);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CInspection);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CProductOverview);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CLocalizedText);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CCoordinates);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CExtendedMetaData);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CResultMetaData);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::COverallResultValues);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CUnionTestingType);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CCDMResultVarRecursive);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CCDMResultVarString);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CCDMResult);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CPointsInCoordinateFormat);
Q_DECLARE_METATYPE(sdl::V1_0::complextest::CCollectionViewParamsTest);
