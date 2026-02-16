#pragma once


#ifdef QT_QML_LIB
#include <QtQml/QQmlEngine>
#endif // QT_QML_LIB

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QMetaEnum>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtsdl/TListModelBase.h>
#include <imtsdl/TElementList.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>

// custom types includes
#include <complextestsdl/SDL/1.0/CPP/ComplexUnion1.h>
#include <imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace sdl::complextest::ComplexUnion2
{


Q_NAMESPACE

// forward declaration for union types
class ExtendedMetaDataUnionType;
class CExtendedMetaDataUnionTypeObject;
class CExtendedMetaDataUnionTypeObjectList;
class ExtendedMetaDataUnionSimpleType;
class CExtendedMetaDataUnionSimpleTypeObject;
class CExtendedMetaDataUnionSimpleTypeObjectList;
class CDMResultUnionType;
class CCDMResultUnionTypeObject;
class CCDMResultUnionTypeObjectList;

class CResult
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
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

		istd::TSharedNullable<ComplexUnion1::StatusCode> AreaResult;
		istd::TSharedNullable<double> ExpectedMinValue;
		istd::TSharedNullable<double> ExpectedMaxValue;
		istd::TSharedNullable<double> MeasuredValue;
		istd::TSharedNullable<double> MinMeasuredValue;
		istd::TSharedNullable<double> MaxMeasuredValue;
		istd::TSharedNullable<double> MeanMeasuredValue;
		istd::TSharedNullable<ComplexUnion1::MeasurementType> MeasurementType;
		istd::TSharedNullable<ComplexUnion1::MeasurementUnit> MeasurementUnit;
		istd::TSharedNullable<double> Length;
		istd::TSharedNullable<ComplexUnion1::ErrorCode> ErrorType;
		istd::TSharedNullable<ComplexUnion1::CGeometry::V1_0> Geometry;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CArea
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct AreaFields
		{
			static const inline QString AreaName = "AreaName";
			static const inline QString Status = "Status";
			static const inline QString IconPosition = "IconPosition";
			static const inline QString ErrorType = "ErrorType";
			static const inline QString Results = "Results";
		};

		istd::TSharedNullable<QString> AreaName;
		istd::TSharedNullable<ComplexUnion1::StatusCode> Status;
		istd::TSharedNullable<ComplexUnion1::CPoint::V1_0> IconPosition;
		istd::TSharedNullable<ComplexUnion1::ErrorCode> ErrorType;
		istd::TSharedNullable<imtsdl::TElementList<CResult::V1_0>> Results;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CAnalyzer
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct AnalyzerFields
		{
			static const inline QString AnalyzerName = "AnalyzerName";
			static const inline QString AnalyzerResult = "AnalyzerResult";
			static const inline QString Areas = "Areas";
		};

		istd::TSharedNullable<QString> AnalyzerName;
		istd::TSharedNullable<ComplexUnion1::StatusCode> AnalyzerResult;
		istd::TSharedNullable<imtsdl::TElementList<CArea::V1_0>> Areas;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CInspection
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct InspectionFields
		{
			static const inline QString ID = "ID";
			static const inline QString TypeID = "TypeID";
			static const inline QString Name = "Name";
			static const inline QString Status = "Status";
			static const inline QString Analyzers = "Analyzers";
		};

		istd::TSharedNullable<QString> ID;
		istd::TSharedNullable<QString> TypeID;
		istd::TSharedNullable<QString> Name;
		istd::TSharedNullable<ComplexUnion1::StatusCode> Status;
		istd::TSharedNullable<imtsdl::TElementList<CAnalyzer::V1_0>> Analyzers;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CProductOverview
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
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

		istd::TSharedNullable<int> ProductID;
		istd::TSharedNullable<QString> Name;
		istd::TSharedNullable<ComplexUnion1::StatusCode> Status;
		istd::TSharedNullable<QString> PartSerialNumber;
		istd::TSharedNullable<QString> Timestamp;
		istd::TSharedNullable<QString> ProductImage;
		istd::TSharedNullable<imtsdl::TElementList<CInspection::V1_0>> Inspections;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CLocalizedText
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct LocalizedTextFields
		{
			static const inline QString Text = "text";
			static const inline QString Locale = "locale";
		};

		istd::TSharedNullable<QString> text;
		istd::TSharedNullable<QString> locale;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CCoordinates
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct CoordinatesFields
		{
			static const inline QString X = "x";
			static const inline QString Y = "y";
			static const inline QString Z = "z";
		};

		istd::TSharedNullable<double> x;
		istd::TSharedNullable<double> y;
		istd::TSharedNullable<double> z;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CExtendedMetaData
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct ExtendedMetaDataFields
		{
			static const inline QString Key = "key";
			static const inline QString Value = "value";
		};

		istd::TSharedNullable<QString> key;
		istd::TSharedNullable<ExtendedMetaDataUnionType> value;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CResultMetaData
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
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

		istd::TSharedNullable<QByteArray> resultId;
		istd::TSharedNullable<QString> creationTime;
		istd::TSharedNullable<QString> partId;
		istd::TSharedNullable<QString> productId;
		istd::TSharedNullable<QString> name;
		istd::TSharedNullable<int> classification;
		istd::TSharedNullable<CLocalizedText::V1_0> description;
		istd::TSharedNullable<imtsdl::TElementList<CExtendedMetaData::V1_0>> extendedMetaData;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class COverallResultValues
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct OverallResultValuesFields
		{
			static const inline QString Name = "name";
			static const inline QString ValueId = "valueId";
			static const inline QString MeasuredValue = "measuredValue";
			static const inline QString LowLimit = "lowLimit";
			static const inline QString HighLimit = "highLimit";
		};

		istd::TSharedNullable<QString> name;
		istd::TSharedNullable<QString> valueId;
		istd::TSharedNullable<double> measuredValue;
		istd::TSharedNullable<double> lowLimit;
		istd::TSharedNullable<double> highLimit;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CUnionTestingType
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct UnionTestingTypeFields
		{
			static const inline QString SimpleUnion = "simpleUnion";
			static const inline QString ComplexUnion = "complexUnion";
			static const inline QString MixedUnion = "mixedUnion";
		};

		istd::TSharedNullable<ExtendedMetaDataUnionSimpleType> simpleUnion;
		istd::TSharedNullable<CDMResultUnionType> complexUnion;
		istd::TSharedNullable<ExtendedMetaDataUnionType> mixedUnion;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CCDMResultVarRecursive
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct CDMResultVarRecursiveFields
		{
			static const inline QString DataModelTitle = "dataModelTitle";
			static const inline QString DataModelVersion = "dataModelVersion";
			static const inline QString ResultMetaData = "resultMetaData";
			static const inline QString ResultContent = "resultContent";
			static const inline QString OverallResultValues = "overallResultValues";
		};

		istd::TSharedNullable<QString> dataModelTitle;
		istd::TSharedNullable<QString> dataModelVersion;
		istd::TSharedNullable<CResultMetaData::V1_0> resultMetaData;
		istd::TSharedNullable<imtsdl::TElementList<CDMResultUnionType>> resultContent;
		istd::TSharedNullable<imtsdl::TElementList<COverallResultValues::V1_0>> overallResultValues;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CCDMResultVarString
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct CDMResultVarStringFields
		{
			static const inline QString DataModelTitle = "dataModelTitle";
			static const inline QString DataModelVersion = "dataModelVersion";
			static const inline QString ResultMetaData = "resultMetaData";
			static const inline QString ResultContent = "resultContent";
			static const inline QString OverallResultValues = "overallResultValues";
		};

		istd::TSharedNullable<QString> dataModelTitle;
		istd::TSharedNullable<QString> dataModelVersion;
		istd::TSharedNullable<CResultMetaData::V1_0> resultMetaData;
		istd::TSharedNullable<imtsdl::TElementList<QString>> resultContent;
		istd::TSharedNullable<imtsdl::TElementList<COverallResultValues::V1_0>> overallResultValues;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CCDMResult
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct CDMResultFields
		{
			static const inline QString DataModelTitle = "dataModelTitle";
			static const inline QString DataModelVersion = "dataModelVersion";
			static const inline QString ResultMetaData = "resultMetaData";
			static const inline QString ResultContent = "resultContent";
			static const inline QString OverallResultValues = "overallResultValues";
		};

		istd::TSharedNullable<QString> dataModelTitle;
		istd::TSharedNullable<QString> dataModelVersion;
		istd::TSharedNullable<CResultMetaData::V1_0> resultMetaData;
		istd::TSharedNullable<imtsdl::TElementList<CDMResultUnionType>> resultContent;
		istd::TSharedNullable<imtsdl::TElementList<COverallResultValues::V1_0>> overallResultValues;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CPointsInCoordinateFormat
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct PointsInCoordinateFormatFields
		{
			static const inline QString PointAsCoordinates = "pointAsCoordinates";
		};

		istd::TSharedNullable<bool> pointAsCoordinates;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CCollectionViewParamsTest
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct CollectionViewParamsTestFields
		{
			static const inline QString Count = "count";
			static const inline QString Offset = "offset";
			static const inline QString FilterModel = "filterModel";
		};

		istd::TSharedNullable<int> count;
		istd::TSharedNullable<int> offset;
		istd::TSharedNullable<imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0> filterModel;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};




class CResultObjectList;

class CResultObject: public ::imtbase::CItemModelBase, public CResult
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_areaResult READ GetAreaResult WRITE SetAreaResult NOTIFY areaResultChanged)
	Q_PROPERTY(QVariant m_expectedMinValue READ GetExpectedMinValue WRITE SetExpectedMinValue NOTIFY expectedMinValueChanged)
	Q_PROPERTY(QVariant m_expectedMaxValue READ GetExpectedMaxValue WRITE SetExpectedMaxValue NOTIFY expectedMaxValueChanged)
	Q_PROPERTY(QVariant m_measuredValue READ GetMeasuredValue WRITE SetMeasuredValue NOTIFY measuredValueChanged)
	Q_PROPERTY(QVariant m_minMeasuredValue READ GetMinMeasuredValue WRITE SetMinMeasuredValue NOTIFY minMeasuredValueChanged)
	Q_PROPERTY(QVariant m_maxMeasuredValue READ GetMaxMeasuredValue WRITE SetMaxMeasuredValue NOTIFY maxMeasuredValueChanged)
	Q_PROPERTY(QVariant m_meanMeasuredValue READ GetMeanMeasuredValue WRITE SetMeanMeasuredValue NOTIFY meanMeasuredValueChanged)
	Q_PROPERTY(QVariant m_measurementType READ GetMeasurementType WRITE SetMeasurementType NOTIFY measurementTypeChanged)
	Q_PROPERTY(QVariant m_measurementUnit READ GetMeasurementUnit WRITE SetMeasurementUnit NOTIFY measurementUnitChanged)
	Q_PROPERTY(QVariant m_length READ GetLength WRITE SetLength NOTIFY lengthChanged)
	Q_PROPERTY(QVariant m_errorType READ GetErrorType WRITE SetErrorType NOTIFY errorTypeChanged)
	Q_PROPERTY(QVariant m_geometry READ GetGeometry WRITE SetGeometry NOTIFY geometryChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CResultObject(QObject* parent = nullptr);

	QVariant GetAreaResult();
	void SetAreaResult(const QVariant& v);
	Q_INVOKABLE bool hasAreaResult();
	QVariant GetExpectedMinValue();
	void SetExpectedMinValue(const QVariant& v);
	Q_INVOKABLE bool hasExpectedMinValue();
	QVariant GetExpectedMaxValue();
	void SetExpectedMaxValue(const QVariant& v);
	Q_INVOKABLE bool hasExpectedMaxValue();
	QVariant GetMeasuredValue();
	void SetMeasuredValue(const QVariant& v);
	Q_INVOKABLE bool hasMeasuredValue();
	QVariant GetMinMeasuredValue();
	void SetMinMeasuredValue(const QVariant& v);
	Q_INVOKABLE bool hasMinMeasuredValue();
	QVariant GetMaxMeasuredValue();
	void SetMaxMeasuredValue(const QVariant& v);
	Q_INVOKABLE bool hasMaxMeasuredValue();
	QVariant GetMeanMeasuredValue();
	void SetMeanMeasuredValue(const QVariant& v);
	Q_INVOKABLE bool hasMeanMeasuredValue();
	QVariant GetMeasurementType();
	void SetMeasurementType(const QVariant& v);
	Q_INVOKABLE bool hasMeasurementType();
	QVariant GetMeasurementUnit();
	void SetMeasurementUnit(const QVariant& v);
	Q_INVOKABLE bool hasMeasurementUnit();
	QVariant GetLength();
	void SetLength(const QVariant& v);
	Q_INVOKABLE bool hasLength();
	QVariant GetErrorType();
	void SetErrorType(const QVariant& v);
	Q_INVOKABLE bool hasErrorType();
	QVariant GetGeometry();
	void SetGeometry(const QVariant& v);
	Q_INVOKABLE bool hasGeometry();
	Q_INVOKABLE void emplaceGeometry();
	void ResetGeometry();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void areaResultChanged();
	void expectedMinValueChanged();
	void expectedMaxValueChanged();
	void measuredValueChanged();
	void minMeasuredValueChanged();
	void maxMeasuredValueChanged();
	void meanMeasuredValueChanged();
	void measurementTypeChanged();
	void measurementUnitChanged();
	void lengthChanged();
	void errorTypeChanged();
	void geometryChanged();

protected:
	QVariant m_geometryQObjectPtr;
};





class CResultObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CResult::V1_0, sdl::complextest::ComplexUnion2::CResultObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CResult::V1_0, sdl::complextest::ComplexUnion2::CResultObject> BaseClass;

	explicit CResultObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CResultObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CResultObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CResultObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CResultObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CResultObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CAreaObjectList;

class CAreaObject: public ::imtbase::CItemModelBase, public CArea
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_areaName READ GetAreaName WRITE SetAreaName NOTIFY areaNameChanged)
	Q_PROPERTY(QVariant m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QVariant m_iconPosition READ GetIconPosition WRITE SetIconPosition NOTIFY iconPositionChanged)
	Q_PROPERTY(QVariant m_errorType READ GetErrorType WRITE SetErrorType NOTIFY errorTypeChanged)
	Q_PROPERTY(QVariant m_results READ GetResults WRITE SetResults NOTIFY resultsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CAreaObject(QObject* parent = nullptr);

	QVariant GetAreaName();
	void SetAreaName(const QVariant& v);
	Q_INVOKABLE bool hasAreaName();
	QVariant GetStatus();
	void SetStatus(const QVariant& v);
	Q_INVOKABLE bool hasStatus();
	QVariant GetIconPosition();
	void SetIconPosition(const QVariant& v);
	Q_INVOKABLE bool hasIconPosition();
	Q_INVOKABLE void emplaceIconPosition();
	void ResetIconPosition();
	QVariant GetErrorType();
	void SetErrorType(const QVariant& v);
	Q_INVOKABLE bool hasErrorType();
	QVariant GetResults();
	void SetResults(const QVariant& v);
	Q_INVOKABLE bool hasResults();
	Q_INVOKABLE void emplaceResults();
	void ResetResults();
	Q_INVOKABLE QVariant createResultsArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void areaNameChanged();
	void statusChanged();
	void iconPositionChanged();
	void errorTypeChanged();
	void resultsChanged();

protected:
	QVariant m_iconPositionQObjectPtr;
	QVariant m_resultsQObjectPtr;
};





class CAreaObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CArea::V1_0, sdl::complextest::ComplexUnion2::CAreaObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CArea::V1_0, sdl::complextest::ComplexUnion2::CAreaObject> BaseClass;

	explicit CAreaObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CAreaObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CAreaObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CAreaObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CAreaObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CAreaObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CAnalyzerObjectList;

class CAnalyzerObject: public ::imtbase::CItemModelBase, public CAnalyzer
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_analyzerName READ GetAnalyzerName WRITE SetAnalyzerName NOTIFY analyzerNameChanged)
	Q_PROPERTY(QVariant m_analyzerResult READ GetAnalyzerResult WRITE SetAnalyzerResult NOTIFY analyzerResultChanged)
	Q_PROPERTY(QVariant m_areas READ GetAreas WRITE SetAreas NOTIFY areasChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CAnalyzerObject(QObject* parent = nullptr);

	QVariant GetAnalyzerName();
	void SetAnalyzerName(const QVariant& v);
	Q_INVOKABLE bool hasAnalyzerName();
	QVariant GetAnalyzerResult();
	void SetAnalyzerResult(const QVariant& v);
	Q_INVOKABLE bool hasAnalyzerResult();
	QVariant GetAreas();
	void SetAreas(const QVariant& v);
	Q_INVOKABLE bool hasAreas();
	Q_INVOKABLE void emplaceAreas();
	void ResetAreas();
	Q_INVOKABLE QVariant createAreasArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void analyzerNameChanged();
	void analyzerResultChanged();
	void areasChanged();

protected:
	QVariant m_areasQObjectPtr;
};





class CAnalyzerObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CAnalyzer::V1_0, sdl::complextest::ComplexUnion2::CAnalyzerObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CAnalyzer::V1_0, sdl::complextest::ComplexUnion2::CAnalyzerObject> BaseClass;

	explicit CAnalyzerObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CAnalyzerObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CAnalyzerObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CAnalyzerObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CAnalyzerObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CAnalyzerObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CInspectionObjectList;

class CInspectionObject: public ::imtbase::CItemModelBase, public CInspection
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_iD READ GetID WRITE SetID NOTIFY iDChanged)
	Q_PROPERTY(QVariant m_typeID READ GetTypeID WRITE SetTypeID NOTIFY typeIDChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QVariant m_analyzers READ GetAnalyzers WRITE SetAnalyzers NOTIFY analyzersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CInspectionObject(QObject* parent = nullptr);

	QVariant GetID();
	void SetID(const QVariant& v);
	Q_INVOKABLE bool hasID();
	QVariant GetTypeID();
	void SetTypeID(const QVariant& v);
	Q_INVOKABLE bool hasTypeID();
	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetStatus();
	void SetStatus(const QVariant& v);
	Q_INVOKABLE bool hasStatus();
	QVariant GetAnalyzers();
	void SetAnalyzers(const QVariant& v);
	Q_INVOKABLE bool hasAnalyzers();
	Q_INVOKABLE void emplaceAnalyzers();
	void ResetAnalyzers();
	Q_INVOKABLE QVariant createAnalyzersArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void iDChanged();
	void typeIDChanged();
	void nameChanged();
	void statusChanged();
	void analyzersChanged();

protected:
	QVariant m_analyzersQObjectPtr;
};





class CInspectionObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CInspection::V1_0, sdl::complextest::ComplexUnion2::CInspectionObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CInspection::V1_0, sdl::complextest::ComplexUnion2::CInspectionObject> BaseClass;

	explicit CInspectionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CInspectionObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CInspectionObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CInspectionObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CInspectionObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CInspectionObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CProductOverviewObjectList;

class CProductOverviewObject: public ::imtbase::CItemModelBase, public CProductOverview
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_productID READ GetProductID WRITE SetProductID NOTIFY productIDChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QVariant m_partSerialNumber READ GetPartSerialNumber WRITE SetPartSerialNumber NOTIFY partSerialNumberChanged)
	Q_PROPERTY(QVariant m_timestamp READ GetTimestamp WRITE SetTimestamp NOTIFY timestampChanged)
	Q_PROPERTY(QVariant m_productImage READ GetProductImage WRITE SetProductImage NOTIFY productImageChanged)
	Q_PROPERTY(QVariant m_inspections READ GetInspections WRITE SetInspections NOTIFY inspectionsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CProductOverviewObject(QObject* parent = nullptr);

	QVariant GetProductID();
	void SetProductID(const QVariant& v);
	Q_INVOKABLE bool hasProductID();
	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetStatus();
	void SetStatus(const QVariant& v);
	Q_INVOKABLE bool hasStatus();
	QVariant GetPartSerialNumber();
	void SetPartSerialNumber(const QVariant& v);
	Q_INVOKABLE bool hasPartSerialNumber();
	QVariant GetTimestamp();
	void SetTimestamp(const QVariant& v);
	Q_INVOKABLE bool hasTimestamp();
	QVariant GetProductImage();
	void SetProductImage(const QVariant& v);
	Q_INVOKABLE bool hasProductImage();
	QVariant GetInspections();
	void SetInspections(const QVariant& v);
	Q_INVOKABLE bool hasInspections();
	Q_INVOKABLE void emplaceInspections();
	void ResetInspections();
	Q_INVOKABLE QVariant createInspectionsArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void productIDChanged();
	void nameChanged();
	void statusChanged();
	void partSerialNumberChanged();
	void timestampChanged();
	void productImageChanged();
	void inspectionsChanged();

protected:
	QVariant m_inspectionsQObjectPtr;
};





class CProductOverviewObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CProductOverview::V1_0, sdl::complextest::ComplexUnion2::CProductOverviewObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CProductOverview::V1_0, sdl::complextest::ComplexUnion2::CProductOverviewObject> BaseClass;

	explicit CProductOverviewObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CProductOverviewObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CProductOverviewObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CProductOverviewObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CProductOverviewObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CProductOverviewObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CLocalizedTextObjectList;

class CLocalizedTextObject: public ::imtbase::CItemModelBase, public CLocalizedText
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_text READ GetText WRITE SetText NOTIFY textChanged)
	Q_PROPERTY(QVariant m_locale READ GetLocale WRITE SetLocale NOTIFY localeChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CLocalizedTextObject(QObject* parent = nullptr);

	QVariant GetText();
	void SetText(const QVariant& v);
	Q_INVOKABLE bool hasText();
	QVariant GetLocale();
	void SetLocale(const QVariant& v);
	Q_INVOKABLE bool hasLocale();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void textChanged();
	void localeChanged();

protected:
};





class CLocalizedTextObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CLocalizedText::V1_0, sdl::complextest::ComplexUnion2::CLocalizedTextObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CLocalizedText::V1_0, sdl::complextest::ComplexUnion2::CLocalizedTextObject> BaseClass;

	explicit CLocalizedTextObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CLocalizedTextObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CLocalizedTextObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CLocalizedTextObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CLocalizedTextObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CLocalizedTextObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CCoordinatesObjectList;

class CCoordinatesObject: public ::imtbase::CItemModelBase, public CCoordinates
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(QVariant m_y READ GetY WRITE SetY NOTIFY yChanged)
	Q_PROPERTY(QVariant m_z READ GetZ WRITE SetZ NOTIFY zChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCoordinatesObject(QObject* parent = nullptr);

	QVariant GetX();
	void SetX(const QVariant& v);
	Q_INVOKABLE bool hasX();
	QVariant GetY();
	void SetY(const QVariant& v);
	Q_INVOKABLE bool hasY();
	QVariant GetZ();
	void SetZ(const QVariant& v);
	Q_INVOKABLE bool hasZ();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void xChanged();
	void yChanged();
	void zChanged();

protected:
};





class CCoordinatesObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCoordinates::V1_0, sdl::complextest::ComplexUnion2::CCoordinatesObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCoordinates::V1_0, sdl::complextest::ComplexUnion2::CCoordinatesObject> BaseClass;

	explicit CCoordinatesObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCoordinatesObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCoordinatesObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCoordinatesObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCoordinatesObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCoordinatesObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CExtendedMetaDataObjectList;

class CExtendedMetaDataObject: public ::imtbase::CItemModelBase, public CExtendedMetaData
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_key READ GetKey WRITE SetKey NOTIFY keyChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CExtendedMetaDataObject(QObject* parent = nullptr);

	QVariant GetKey();
	void SetKey(const QVariant& v);
	Q_INVOKABLE bool hasKey();
	QVariant GetValue();
	void SetValue(const QVariant& v);
	Q_INVOKABLE bool hasValue();
	Q_INVOKABLE void emplaceValue();
	void ResetValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void keyChanged();
	void valueChanged();

protected:
	QVariant m_valueQObjectPtr;
};





class CExtendedMetaDataObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CExtendedMetaData::V1_0, sdl::complextest::ComplexUnion2::CExtendedMetaDataObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CExtendedMetaData::V1_0, sdl::complextest::ComplexUnion2::CExtendedMetaDataObject> BaseClass;

	explicit CExtendedMetaDataObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CResultMetaDataObjectList;

class CResultMetaDataObject: public ::imtbase::CItemModelBase, public CResultMetaData
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_resultId READ GetResultId WRITE SetResultId NOTIFY resultIdChanged)
	Q_PROPERTY(QVariant m_creationTime READ GetCreationTime WRITE SetCreationTime NOTIFY creationTimeChanged)
	Q_PROPERTY(QVariant m_partId READ GetPartId WRITE SetPartId NOTIFY partIdChanged)
	Q_PROPERTY(QVariant m_productId READ GetProductId WRITE SetProductId NOTIFY productIdChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_classification READ GetClassification WRITE SetClassification NOTIFY classificationChanged)
	Q_PROPERTY(QVariant m_description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QVariant m_extendedMetaData READ GetExtendedMetaData WRITE SetExtendedMetaData NOTIFY extendedMetaDataChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CResultMetaDataObject(QObject* parent = nullptr);

	QVariant GetResultId();
	void SetResultId(const QVariant& v);
	Q_INVOKABLE bool hasResultId();
	QVariant GetCreationTime();
	void SetCreationTime(const QVariant& v);
	Q_INVOKABLE bool hasCreationTime();
	QVariant GetPartId();
	void SetPartId(const QVariant& v);
	Q_INVOKABLE bool hasPartId();
	QVariant GetProductId();
	void SetProductId(const QVariant& v);
	Q_INVOKABLE bool hasProductId();
	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetClassification();
	void SetClassification(const QVariant& v);
	Q_INVOKABLE bool hasClassification();
	QVariant GetDescription();
	void SetDescription(const QVariant& v);
	Q_INVOKABLE bool hasDescription();
	Q_INVOKABLE void emplaceDescription();
	void ResetDescription();
	QVariant GetExtendedMetaData();
	void SetExtendedMetaData(const QVariant& v);
	Q_INVOKABLE bool hasExtendedMetaData();
	Q_INVOKABLE void emplaceExtendedMetaData();
	void ResetExtendedMetaData();
	Q_INVOKABLE QVariant createExtendedMetaDataArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void resultIdChanged();
	void creationTimeChanged();
	void partIdChanged();
	void productIdChanged();
	void nameChanged();
	void classificationChanged();
	void descriptionChanged();
	void extendedMetaDataChanged();

protected:
	QVariant m_descriptionQObjectPtr;
	QVariant m_extendedMetaDataQObjectPtr;
};





class CResultMetaDataObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CResultMetaData::V1_0, sdl::complextest::ComplexUnion2::CResultMetaDataObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CResultMetaData::V1_0, sdl::complextest::ComplexUnion2::CResultMetaDataObject> BaseClass;

	explicit CResultMetaDataObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CResultMetaDataObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CResultMetaDataObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CResultMetaDataObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CResultMetaDataObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CResultMetaDataObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class COverallResultValuesObjectList;

class COverallResultValuesObject: public ::imtbase::CItemModelBase, public COverallResultValues
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_valueId READ GetValueId WRITE SetValueId NOTIFY valueIdChanged)
	Q_PROPERTY(QVariant m_measuredValue READ GetMeasuredValue WRITE SetMeasuredValue NOTIFY measuredValueChanged)
	Q_PROPERTY(QVariant m_lowLimit READ GetLowLimit WRITE SetLowLimit NOTIFY lowLimitChanged)
	Q_PROPERTY(QVariant m_highLimit READ GetHighLimit WRITE SetHighLimit NOTIFY highLimitChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COverallResultValuesObject(QObject* parent = nullptr);

	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetValueId();
	void SetValueId(const QVariant& v);
	Q_INVOKABLE bool hasValueId();
	QVariant GetMeasuredValue();
	void SetMeasuredValue(const QVariant& v);
	Q_INVOKABLE bool hasMeasuredValue();
	QVariant GetLowLimit();
	void SetLowLimit(const QVariant& v);
	Q_INVOKABLE bool hasLowLimit();
	QVariant GetHighLimit();
	void SetHighLimit(const QVariant& v);
	Q_INVOKABLE bool hasHighLimit();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void nameChanged();
	void valueIdChanged();
	void measuredValueChanged();
	void lowLimitChanged();
	void highLimitChanged();

protected:
};





class COverallResultValuesObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::COverallResultValues::V1_0, sdl::complextest::ComplexUnion2::COverallResultValuesObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::COverallResultValues::V1_0, sdl::complextest::ComplexUnion2::COverallResultValuesObject> BaseClass;

	explicit COverallResultValuesObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::COverallResultValuesObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::COverallResultValuesObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::COverallResultValuesObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CUnionTestingTypeObjectList;

class CUnionTestingTypeObject: public ::imtbase::CItemModelBase, public CUnionTestingType
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_simpleUnion READ GetSimpleUnion WRITE SetSimpleUnion NOTIFY simpleUnionChanged)
	Q_PROPERTY(QVariant m_complexUnion READ GetComplexUnion WRITE SetComplexUnion NOTIFY complexUnionChanged)
	Q_PROPERTY(QVariant m_mixedUnion READ GetMixedUnion WRITE SetMixedUnion NOTIFY mixedUnionChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CUnionTestingTypeObject(QObject* parent = nullptr);

	QVariant GetSimpleUnion();
	void SetSimpleUnion(const QVariant& v);
	Q_INVOKABLE bool hasSimpleUnion();
	Q_INVOKABLE void emplaceSimpleUnion();
	void ResetSimpleUnion();
	QVariant GetComplexUnion();
	void SetComplexUnion(const QVariant& v);
	Q_INVOKABLE bool hasComplexUnion();
	Q_INVOKABLE void emplaceComplexUnion();
	void ResetComplexUnion();
	QVariant GetMixedUnion();
	void SetMixedUnion(const QVariant& v);
	Q_INVOKABLE bool hasMixedUnion();
	Q_INVOKABLE void emplaceMixedUnion();
	void ResetMixedUnion();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void simpleUnionChanged();
	void complexUnionChanged();
	void mixedUnionChanged();

protected:
	QVariant m_simpleUnionQObjectPtr;
	QVariant m_complexUnionQObjectPtr;
	QVariant m_mixedUnionQObjectPtr;
};





class CUnionTestingTypeObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CUnionTestingType::V1_0, sdl::complextest::ComplexUnion2::CUnionTestingTypeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CUnionTestingType::V1_0, sdl::complextest::ComplexUnion2::CUnionTestingTypeObject> BaseClass;

	explicit CUnionTestingTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CUnionTestingTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CUnionTestingTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CCDMResultVarRecursiveObjectList;

class CCDMResultVarRecursiveObject: public ::imtbase::CItemModelBase, public CCDMResultVarRecursive
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QVariant m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(QVariant m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(QVariant m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(QVariant m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultVarRecursiveObject(QObject* parent = nullptr);

	QVariant GetDataModelTitle();
	void SetDataModelTitle(const QVariant& v);
	Q_INVOKABLE bool hasDataModelTitle();
	QVariant GetDataModelVersion();
	void SetDataModelVersion(const QVariant& v);
	Q_INVOKABLE bool hasDataModelVersion();
	QVariant GetResultMetaData();
	void SetResultMetaData(const QVariant& v);
	Q_INVOKABLE bool hasResultMetaData();
	Q_INVOKABLE void emplaceResultMetaData();
	void ResetResultMetaData();
	QVariant GetResultContent();
	void SetResultContent(const QVariant& v);
	Q_INVOKABLE bool hasResultContent();
	Q_INVOKABLE void emplaceResultContent();
	void ResetResultContent();
	QVariant GetOverallResultValues();
	void SetOverallResultValues(const QVariant& v);
	Q_INVOKABLE bool hasOverallResultValues();
	Q_INVOKABLE void emplaceOverallResultValues();
	void ResetOverallResultValues();
	Q_INVOKABLE QVariant createOverallResultValuesArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataModelTitleChanged();
	void dataModelVersionChanged();
	void resultMetaDataChanged();
	void resultContentChanged();
	void overallResultValuesChanged();

protected:
	QVariant m_resultMetaDataQObjectPtr;
	QVariant m_resultContentQObjectPtr;
	QVariant m_overallResultValuesQObjectPtr;
};





class CCDMResultVarRecursiveObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarRecursive::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarRecursive::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject> BaseClass;

	explicit CCDMResultVarRecursiveObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CCDMResultVarStringObjectList;

class CCDMResultVarStringObject: public ::imtbase::CItemModelBase, public CCDMResultVarString
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QVariant m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(QVariant m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(QVariant m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(QVariant m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultVarStringObject(QObject* parent = nullptr);

	QVariant GetDataModelTitle();
	void SetDataModelTitle(const QVariant& v);
	Q_INVOKABLE bool hasDataModelTitle();
	QVariant GetDataModelVersion();
	void SetDataModelVersion(const QVariant& v);
	Q_INVOKABLE bool hasDataModelVersion();
	QVariant GetResultMetaData();
	void SetResultMetaData(const QVariant& v);
	Q_INVOKABLE bool hasResultMetaData();
	Q_INVOKABLE void emplaceResultMetaData();
	void ResetResultMetaData();
	QVariant GetResultContent();
	void SetResultContent(const QVariant& v);
	Q_INVOKABLE bool hasResultContent();
	QVariant GetOverallResultValues();
	void SetOverallResultValues(const QVariant& v);
	Q_INVOKABLE bool hasOverallResultValues();
	Q_INVOKABLE void emplaceOverallResultValues();
	void ResetOverallResultValues();
	Q_INVOKABLE QVariant createOverallResultValuesArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataModelTitleChanged();
	void dataModelVersionChanged();
	void resultMetaDataChanged();
	void resultContentChanged();
	void overallResultValuesChanged();

protected:
	QVariant m_resultMetaDataQObjectPtr;
	QVariant m_overallResultValuesQObjectPtr;
};





class CCDMResultVarStringObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarString::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarStringObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarString::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarStringObject> BaseClass;

	explicit CCDMResultVarStringObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCDMResultVarStringObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCDMResultVarStringObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CCDMResultObjectList;

class CCDMResultObject: public ::imtbase::CItemModelBase, public CCDMResult
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QVariant m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(QVariant m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(QVariant m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(QVariant m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultObject(QObject* parent = nullptr);

	QVariant GetDataModelTitle();
	void SetDataModelTitle(const QVariant& v);
	Q_INVOKABLE bool hasDataModelTitle();
	QVariant GetDataModelVersion();
	void SetDataModelVersion(const QVariant& v);
	Q_INVOKABLE bool hasDataModelVersion();
	QVariant GetResultMetaData();
	void SetResultMetaData(const QVariant& v);
	Q_INVOKABLE bool hasResultMetaData();
	Q_INVOKABLE void emplaceResultMetaData();
	void ResetResultMetaData();
	QVariant GetResultContent();
	void SetResultContent(const QVariant& v);
	Q_INVOKABLE bool hasResultContent();
	Q_INVOKABLE void emplaceResultContent();
	void ResetResultContent();
	QVariant GetOverallResultValues();
	void SetOverallResultValues(const QVariant& v);
	Q_INVOKABLE bool hasOverallResultValues();
	Q_INVOKABLE void emplaceOverallResultValues();
	void ResetOverallResultValues();
	Q_INVOKABLE QVariant createOverallResultValuesArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataModelTitleChanged();
	void dataModelVersionChanged();
	void resultMetaDataChanged();
	void resultContentChanged();
	void overallResultValuesChanged();

protected:
	QVariant m_resultMetaDataQObjectPtr;
	QVariant m_resultContentQObjectPtr;
	QVariant m_overallResultValuesQObjectPtr;
};





class CCDMResultObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResult::V1_0, sdl::complextest::ComplexUnion2::CCDMResultObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResult::V1_0, sdl::complextest::ComplexUnion2::CCDMResultObject> BaseClass;

	explicit CCDMResultObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCDMResultObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCDMResultObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCDMResultObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCDMResultObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCDMResultObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CPointsInCoordinateFormatObjectList;

class CPointsInCoordinateFormatObject: public ::imtbase::CItemModelBase, public CPointsInCoordinateFormat
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_pointAsCoordinates READ GetPointAsCoordinates WRITE SetPointAsCoordinates NOTIFY pointAsCoordinatesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPointsInCoordinateFormatObject(QObject* parent = nullptr);

	QVariant GetPointAsCoordinates();
	void SetPointAsCoordinates(const QVariant& v);
	Q_INVOKABLE bool hasPointAsCoordinates();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void pointAsCoordinatesChanged();

protected:
};





class CPointsInCoordinateFormatObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormat::V1_0, sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormat::V1_0, sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject> BaseClass;

	explicit CPointsInCoordinateFormatObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CCollectionViewParamsTestObjectList;

class CCollectionViewParamsTestObject: public ::imtbase::CItemModelBase, public CCollectionViewParamsTest
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_count READ GetCount WRITE SetCount NOTIFY countChanged)
	Q_PROPERTY(QVariant m_offset READ GetOffset WRITE SetOffset NOTIFY offsetChanged)
	Q_PROPERTY(QVariant m_filterModel READ GetFilterModel WRITE SetFilterModel NOTIFY filterModelChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCollectionViewParamsTestObject(QObject* parent = nullptr);

	QVariant GetCount();
	void SetCount(const QVariant& v);
	Q_INVOKABLE bool hasCount();
	QVariant GetOffset();
	void SetOffset(const QVariant& v);
	Q_INVOKABLE bool hasOffset();
	QVariant GetFilterModel();
	void SetFilterModel(const QVariant& v);
	Q_INVOKABLE bool hasFilterModel();
	Q_INVOKABLE void emplaceFilterModel();
	void ResetFilterModel();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void countChanged();
	void offsetChanged();
	void filterModelChanged();

protected:
	QVariant m_filterModelQObjectPtr;
};





class CCollectionViewParamsTestObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCollectionViewParamsTest::V1_0, sdl::complextest::ComplexUnion2::CCollectionViewParamsTestObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion2::CCollectionViewParamsTest::V1_0, sdl::complextest::ComplexUnion2::CCollectionViewParamsTestObject> BaseClass;

	explicit CCollectionViewParamsTestObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCollectionViewParamsTestObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCollectionViewParamsTestObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
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




class CExtendedMetaDataUnionTypeObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::complextest::ComplexUnion2::ExtendedMetaDataUnionType, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::complextest::ComplexUnion2::ExtendedMetaDataUnionType, QVariant> BaseClass;

	CExtendedMetaDataUnionTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};



class CExtendedMetaDataUnionSimpleTypeObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::complextest::ComplexUnion2::ExtendedMetaDataUnionSimpleType, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::complextest::ComplexUnion2::ExtendedMetaDataUnionSimpleType, QVariant> BaseClass;

	CExtendedMetaDataUnionSimpleTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionSimpleTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionSimpleTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};



class CCDMResultUnionTypeObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::complextest::ComplexUnion2::CDMResultUnionType, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::complextest::ComplexUnion2::CDMResultUnionType, QVariant> BaseClass;

	CCDMResultUnionTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
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
	CPointsInCoordinateFormat pointAsCoordinates;
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
	virtual CProductOverview OnGetLastProductionResults(const CGetLastProductionResultsGqlRequest& getLastProductionResultsRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CCDMResult OnGetLastProductionResultsCDM(const CGetLastProductionResultsCDMGqlRequest& getLastProductionResultsCDMRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};




#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
{
	qmlRegisterType<CResultObject>("complextestComplexUnion2Sdl", 1, 0, "Result");
	qmlRegisterType<CAreaObject>("complextestComplexUnion2Sdl", 1, 0, "Area");
	qmlRegisterType<CAnalyzerObject>("complextestComplexUnion2Sdl", 1, 0, "Analyzer");
	qmlRegisterType<CInspectionObject>("complextestComplexUnion2Sdl", 1, 0, "Inspection");
	qmlRegisterType<CProductOverviewObject>("complextestComplexUnion2Sdl", 1, 0, "ProductOverview");
	qmlRegisterType<CLocalizedTextObject>("complextestComplexUnion2Sdl", 1, 0, "LocalizedText");
	qmlRegisterType<CCoordinatesObject>("complextestComplexUnion2Sdl", 1, 0, "Coordinates");
	qmlRegisterType<CExtendedMetaDataObject>("complextestComplexUnion2Sdl", 1, 0, "ExtendedMetaData");
	qmlRegisterType<CResultMetaDataObject>("complextestComplexUnion2Sdl", 1, 0, "ResultMetaData");
	qmlRegisterType<COverallResultValuesObject>("complextestComplexUnion2Sdl", 1, 0, "OverallResultValues");
	qmlRegisterType<CUnionTestingTypeObject>("complextestComplexUnion2Sdl", 1, 0, "UnionTestingType");
	qmlRegisterType<CCDMResultVarRecursiveObject>("complextestComplexUnion2Sdl", 1, 0, "CDMResultVarRecursive");
	qmlRegisterType<CCDMResultVarStringObject>("complextestComplexUnion2Sdl", 1, 0, "CDMResultVarString");
	qmlRegisterType<CCDMResultObject>("complextestComplexUnion2Sdl", 1, 0, "CDMResult");
	qmlRegisterType<CPointsInCoordinateFormatObject>("complextestComplexUnion2Sdl", 1, 0, "PointsInCoordinateFormat");
	qmlRegisterType<CCollectionViewParamsTestObject>("complextestComplexUnion2Sdl", 1, 0, "CollectionViewParamsTest");
}
#endif

} // namespace sdl::complextest::ComplexUnion2


Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CResult::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CResult);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CArea::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CArea);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CAnalyzer::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CAnalyzer);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CInspection::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CInspection);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CProductOverview::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CProductOverview);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CLocalizedText::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CLocalizedText);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCoordinates::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCoordinates);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CExtendedMetaData::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CExtendedMetaData);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CResultMetaData::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CResultMetaData);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::COverallResultValues::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::COverallResultValues);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CUnionTestingType::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CUnionTestingType);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCDMResultVarRecursive::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCDMResultVarRecursive);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCDMResultVarString::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCDMResultVarString);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCDMResult::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCDMResult);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormat::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormat);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCollectionViewParamsTest::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion2::CCollectionViewParamsTest);
