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
#include <imtbase/TListModelBase.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>

// custom types includes
#include <complextestsdl/SDL/1.0/CPP/ComplexUnion1.h>


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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<CResult::V1_0>> Results;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<CArea::V1_0>> Areas;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<CAnalyzer::V1_0>> Analyzers;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<CInspection::V1_0>> Inspections;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<std::shared_ptr<ExtendedMetaDataUnionType>> value;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<CExtendedMetaData::V1_0>> extendedMetaData;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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

		istd::TSharedNullable<std::shared_ptr<ExtendedMetaDataUnionSimpleType>> simpleUnion;
		istd::TSharedNullable<std::shared_ptr<CDMResultUnionType>> complexUnion;
		istd::TSharedNullable<std::shared_ptr<ExtendedMetaDataUnionType>> mixedUnion;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<std::shared_ptr<CDMResultUnionType>>> resultContent;
		istd::TSharedNullable<QList<COverallResultValues::V1_0>> overallResultValues;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<QString>> resultContent;
		istd::TSharedNullable<QList<COverallResultValues::V1_0>> overallResultValues;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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
		istd::TSharedNullable<QList<std::shared_ptr<CDMResultUnionType>>> resultContent;
		istd::TSharedNullable<QList<COverallResultValues::V1_0>> overallResultValues;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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




class CResultObjectList;

class CResultObject: public ::imtbase::CItemModelBase, public CResult
{
	Q_OBJECT
	Q_PROPERTY(QString m_areaResult READ GetAreaResult WRITE SetAreaResult NOTIFY areaResultChanged)
	Q_PROPERTY(double m_expectedMinValue READ GetExpectedMinValue WRITE SetExpectedMinValue NOTIFY expectedMinValueChanged)
	Q_PROPERTY(double m_expectedMaxValue READ GetExpectedMaxValue WRITE SetExpectedMaxValue NOTIFY expectedMaxValueChanged)
	Q_PROPERTY(double m_measuredValue READ GetMeasuredValue WRITE SetMeasuredValue NOTIFY measuredValueChanged)
	Q_PROPERTY(double m_minMeasuredValue READ GetMinMeasuredValue WRITE SetMinMeasuredValue NOTIFY minMeasuredValueChanged)
	Q_PROPERTY(double m_maxMeasuredValue READ GetMaxMeasuredValue WRITE SetMaxMeasuredValue NOTIFY maxMeasuredValueChanged)
	Q_PROPERTY(double m_meanMeasuredValue READ GetMeanMeasuredValue WRITE SetMeanMeasuredValue NOTIFY meanMeasuredValueChanged)
	Q_PROPERTY(QString m_measurementType READ GetMeasurementType WRITE SetMeasurementType NOTIFY measurementTypeChanged)
	Q_PROPERTY(QString m_measurementUnit READ GetMeasurementUnit WRITE SetMeasurementUnit NOTIFY measurementUnitChanged)
	Q_PROPERTY(double m_length READ GetLength WRITE SetLength NOTIFY lengthChanged)
	Q_PROPERTY(QString m_errorType READ GetErrorType WRITE SetErrorType NOTIFY errorTypeChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion1::CGeometryObject* m_geometry READ GetGeometry WRITE SetGeometry NOTIFY geometryChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CResultObject(QObject* parent = nullptr);

	QString GetAreaResult();
	void SetAreaResult(QString v);
	Q_INVOKABLE bool hasAreaResult();
	double GetExpectedMinValue();
	void SetExpectedMinValue(double v);
	Q_INVOKABLE bool hasExpectedMinValue();
	double GetExpectedMaxValue();
	void SetExpectedMaxValue(double v);
	Q_INVOKABLE bool hasExpectedMaxValue();
	double GetMeasuredValue();
	void SetMeasuredValue(double v);
	Q_INVOKABLE bool hasMeasuredValue();
	double GetMinMeasuredValue();
	void SetMinMeasuredValue(double v);
	Q_INVOKABLE bool hasMinMeasuredValue();
	double GetMaxMeasuredValue();
	void SetMaxMeasuredValue(double v);
	Q_INVOKABLE bool hasMaxMeasuredValue();
	double GetMeanMeasuredValue();
	void SetMeanMeasuredValue(double v);
	Q_INVOKABLE bool hasMeanMeasuredValue();
	QString GetMeasurementType();
	void SetMeasurementType(QString v);
	Q_INVOKABLE bool hasMeasurementType();
	QString GetMeasurementUnit();
	void SetMeasurementUnit(QString v);
	Q_INVOKABLE bool hasMeasurementUnit();
	double GetLength();
	void SetLength(double v);
	Q_INVOKABLE bool hasLength();
	QString GetErrorType();
	void SetErrorType(QString v);
	Q_INVOKABLE bool hasErrorType();
	sdl::complextest::ComplexUnion1::CGeometryObject* GetGeometry();
	void SetGeometry(sdl::complextest::ComplexUnion1::CGeometryObject* v);
	Q_INVOKABLE bool hasGeometry();
	Q_INVOKABLE void createGeometry();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
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
	void finished();

protected:
	sdl::complextest::ComplexUnion1::CGeometryObject* m_geometryQObjectPtr;
};


class CResultObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CResult::V1_0, sdl::complextest::ComplexUnion2::CResultObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CResult::V1_0, sdl::complextest::ComplexUnion2::CResultObject> BaseClass;

	CResultObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CResultObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CResultObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CResultObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CResultObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CResultObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CResultObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CResultObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CResultObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CResultObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CResultObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CResultObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CResultObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CResultObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CResultObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CResultObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CResultObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CResultObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CResultObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CResultObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CResultObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CResultObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CResultObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CResultObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_areaResult"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).AreaResult.value());
		}
		if (nameId == "m_expectedMinValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ExpectedMinValue.value());
		}
		if (nameId == "m_expectedMaxValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ExpectedMaxValue.value());
		}
		if (nameId == "m_measuredValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).MeasuredValue.value());
		}
		if (nameId == "m_minMeasuredValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).MinMeasuredValue.value());
		}
		if (nameId == "m_maxMeasuredValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).MaxMeasuredValue.value());
		}
		if (nameId == "m_meanMeasuredValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).MeanMeasuredValue.value());
		}
		if (nameId == "m_measurementType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).MeasurementType.value());
		}
		if (nameId == "m_measurementUnit"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).MeasurementUnit.value());
		}
		if (nameId == "m_length"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Length.value());
		}
		if (nameId == "m_errorType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ErrorType.value());
		}
		if (nameId == "m_geometry"){
			sdl::complextest::ComplexUnion2::CResultObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetGeometry());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CAreaObjectList;

class CAreaObject: public ::imtbase::CItemModelBase, public CArea
{
	Q_OBJECT
	Q_PROPERTY(QString m_areaName READ GetAreaName WRITE SetAreaName NOTIFY areaNameChanged)
	Q_PROPERTY(QString m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion1::CPointObject* m_iconPosition READ GetIconPosition WRITE SetIconPosition NOTIFY iconPositionChanged)
	Q_PROPERTY(QString m_errorType READ GetErrorType WRITE SetErrorType NOTIFY errorTypeChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CResultObjectList* m_results READ GetResults WRITE SetResults NOTIFY resultsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CAreaObject(QObject* parent = nullptr);

	QString GetAreaName();
	void SetAreaName(QString v);
	Q_INVOKABLE bool hasAreaName();
	QString GetStatus();
	void SetStatus(QString v);
	Q_INVOKABLE bool hasStatus();
	sdl::complextest::ComplexUnion1::CPointObject* GetIconPosition();
	void SetIconPosition(sdl::complextest::ComplexUnion1::CPointObject* v);
	Q_INVOKABLE bool hasIconPosition();
	Q_INVOKABLE void createIconPosition();
	QString GetErrorType();
	void SetErrorType(QString v);
	Q_INVOKABLE bool hasErrorType();
	sdl::complextest::ComplexUnion2::CResultObjectList* GetResults();
	void SetResults(sdl::complextest::ComplexUnion2::CResultObjectList* v);
	Q_INVOKABLE bool hasResults();
	Q_INVOKABLE void createResults();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void areaNameChanged();
	void statusChanged();
	void iconPositionChanged();
	void errorTypeChanged();
	void resultsChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion1::CPointObject* m_iconPositionQObjectPtr;
	sdl::complextest::ComplexUnion2::CResultObjectList* m_resultsQObjectPtr;
};


class CAreaObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CArea::V1_0, sdl::complextest::ComplexUnion2::CAreaObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CArea::V1_0, sdl::complextest::ComplexUnion2::CAreaObject> BaseClass;

	CAreaObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CAreaObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CAreaObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CAreaObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CAreaObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CAreaObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CAreaObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CAreaObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CAreaObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CAreaObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CAreaObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CAreaObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CAreaObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CAreaObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CAreaObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CAreaObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CAreaObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CAreaObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CAreaObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CAreaObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CAreaObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CAreaObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CAreaObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CAreaObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_areaName"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).AreaName.value());
		}
		if (nameId == "m_status"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Status.value());
		}
		if (nameId == "m_iconPosition"){
			sdl::complextest::ComplexUnion2::CAreaObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetIconPosition());
		}
		if (nameId == "m_errorType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ErrorType.value());
		}
		if (nameId == "m_results"){
			sdl::complextest::ComplexUnion2::CAreaObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetResults());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CAnalyzerObjectList;

class CAnalyzerObject: public ::imtbase::CItemModelBase, public CAnalyzer
{
	Q_OBJECT
	Q_PROPERTY(QString m_analyzerName READ GetAnalyzerName WRITE SetAnalyzerName NOTIFY analyzerNameChanged)
	Q_PROPERTY(QString m_analyzerResult READ GetAnalyzerResult WRITE SetAnalyzerResult NOTIFY analyzerResultChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CAreaObjectList* m_areas READ GetAreas WRITE SetAreas NOTIFY areasChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CAnalyzerObject(QObject* parent = nullptr);

	QString GetAnalyzerName();
	void SetAnalyzerName(QString v);
	Q_INVOKABLE bool hasAnalyzerName();
	QString GetAnalyzerResult();
	void SetAnalyzerResult(QString v);
	Q_INVOKABLE bool hasAnalyzerResult();
	sdl::complextest::ComplexUnion2::CAreaObjectList* GetAreas();
	void SetAreas(sdl::complextest::ComplexUnion2::CAreaObjectList* v);
	Q_INVOKABLE bool hasAreas();
	Q_INVOKABLE void createAreas();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void analyzerNameChanged();
	void analyzerResultChanged();
	void areasChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CAreaObjectList* m_areasQObjectPtr;
};


class CAnalyzerObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CAnalyzer::V1_0, sdl::complextest::ComplexUnion2::CAnalyzerObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CAnalyzer::V1_0, sdl::complextest::ComplexUnion2::CAnalyzerObject> BaseClass;

	CAnalyzerObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CAnalyzerObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CAnalyzerObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CAnalyzerObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CAnalyzerObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CAnalyzerObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CAnalyzerObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CAnalyzerObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CAnalyzerObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CAnalyzerObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CAnalyzerObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CAnalyzerObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CAnalyzerObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CAnalyzerObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CAnalyzerObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CAnalyzerObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CAnalyzerObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CAnalyzerObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CAnalyzerObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CAnalyzerObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CAnalyzerObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CAnalyzerObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CAnalyzerObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CAnalyzerObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_analyzerName"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).AnalyzerName.value());
		}
		if (nameId == "m_analyzerResult"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).AnalyzerResult.value());
		}
		if (nameId == "m_areas"){
			sdl::complextest::ComplexUnion2::CAnalyzerObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetAreas());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CInspectionObjectList;

class CInspectionObject: public ::imtbase::CItemModelBase, public CInspection
{
	Q_OBJECT
	Q_PROPERTY(QString m_iD READ GetID WRITE SetID NOTIFY iDChanged)
	Q_PROPERTY(QString m_typeID READ GetTypeID WRITE SetTypeID NOTIFY typeIDChanged)
	Q_PROPERTY(QString m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QString m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CAnalyzerObjectList* m_analyzers READ GetAnalyzers WRITE SetAnalyzers NOTIFY analyzersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CInspectionObject(QObject* parent = nullptr);

	QString GetID();
	void SetID(QString v);
	Q_INVOKABLE bool hasID();
	QString GetTypeID();
	void SetTypeID(QString v);
	Q_INVOKABLE bool hasTypeID();
	QString GetName();
	void SetName(QString v);
	Q_INVOKABLE bool hasName();
	QString GetStatus();
	void SetStatus(QString v);
	Q_INVOKABLE bool hasStatus();
	sdl::complextest::ComplexUnion2::CAnalyzerObjectList* GetAnalyzers();
	void SetAnalyzers(sdl::complextest::ComplexUnion2::CAnalyzerObjectList* v);
	Q_INVOKABLE bool hasAnalyzers();
	Q_INVOKABLE void createAnalyzers();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void iDChanged();
	void typeIDChanged();
	void nameChanged();
	void statusChanged();
	void analyzersChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CAnalyzerObjectList* m_analyzersQObjectPtr;
};


class CInspectionObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CInspection::V1_0, sdl::complextest::ComplexUnion2::CInspectionObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CInspection::V1_0, sdl::complextest::ComplexUnion2::CInspectionObject> BaseClass;

	CInspectionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CInspectionObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CInspectionObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CInspectionObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CInspectionObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CInspectionObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CInspectionObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CInspectionObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CInspectionObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CInspectionObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CInspectionObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CInspectionObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CInspectionObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CInspectionObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CInspectionObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CInspectionObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CInspectionObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CInspectionObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CInspectionObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CInspectionObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CInspectionObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CInspectionObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CInspectionObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CInspectionObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_iD"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ID.value());
		}
		if (nameId == "m_typeID"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).TypeID.value());
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Name.value());
		}
		if (nameId == "m_status"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Status.value());
		}
		if (nameId == "m_analyzers"){
			sdl::complextest::ComplexUnion2::CInspectionObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetAnalyzers());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CProductOverviewObjectList;

class CProductOverviewObject: public ::imtbase::CItemModelBase, public CProductOverview
{
	Q_OBJECT
	Q_PROPERTY(int m_productID READ GetProductID WRITE SetProductID NOTIFY productIDChanged)
	Q_PROPERTY(QString m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QString m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QString m_partSerialNumber READ GetPartSerialNumber WRITE SetPartSerialNumber NOTIFY partSerialNumberChanged)
	Q_PROPERTY(QString m_timestamp READ GetTimestamp WRITE SetTimestamp NOTIFY timestampChanged)
	Q_PROPERTY(QString m_productImage READ GetProductImage WRITE SetProductImage NOTIFY productImageChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CInspectionObjectList* m_inspections READ GetInspections WRITE SetInspections NOTIFY inspectionsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CProductOverviewObject(QObject* parent = nullptr);

	int GetProductID();
	void SetProductID(int v);
	Q_INVOKABLE bool hasProductID();
	QString GetName();
	void SetName(QString v);
	Q_INVOKABLE bool hasName();
	QString GetStatus();
	void SetStatus(QString v);
	Q_INVOKABLE bool hasStatus();
	QString GetPartSerialNumber();
	void SetPartSerialNumber(QString v);
	Q_INVOKABLE bool hasPartSerialNumber();
	QString GetTimestamp();
	void SetTimestamp(QString v);
	Q_INVOKABLE bool hasTimestamp();
	QString GetProductImage();
	void SetProductImage(QString v);
	Q_INVOKABLE bool hasProductImage();
	sdl::complextest::ComplexUnion2::CInspectionObjectList* GetInspections();
	void SetInspections(sdl::complextest::ComplexUnion2::CInspectionObjectList* v);
	Q_INVOKABLE bool hasInspections();
	Q_INVOKABLE void createInspections();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void productIDChanged();
	void nameChanged();
	void statusChanged();
	void partSerialNumberChanged();
	void timestampChanged();
	void productImageChanged();
	void inspectionsChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CInspectionObjectList* m_inspectionsQObjectPtr;
};


class CProductOverviewObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CProductOverview::V1_0, sdl::complextest::ComplexUnion2::CProductOverviewObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CProductOverview::V1_0, sdl::complextest::ComplexUnion2::CProductOverviewObject> BaseClass;

	CProductOverviewObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CProductOverviewObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CProductOverviewObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CProductOverviewObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CProductOverviewObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CProductOverviewObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CProductOverviewObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CProductOverviewObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CProductOverviewObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CProductOverviewObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CProductOverviewObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CProductOverviewObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CProductOverviewObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CProductOverviewObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CProductOverviewObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CProductOverviewObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CProductOverviewObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CProductOverviewObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CProductOverviewObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CProductOverviewObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CProductOverviewObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CProductOverviewObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CProductOverviewObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CProductOverviewObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_productID"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ProductID.value());
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Name.value());
		}
		if (nameId == "m_status"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Status.value());
		}
		if (nameId == "m_partSerialNumber"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).PartSerialNumber.value());
		}
		if (nameId == "m_timestamp"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Timestamp.value());
		}
		if (nameId == "m_productImage"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ProductImage.value());
		}
		if (nameId == "m_inspections"){
			sdl::complextest::ComplexUnion2::CProductOverviewObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetInspections());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CLocalizedTextObjectList;

class CLocalizedTextObject: public ::imtbase::CItemModelBase, public CLocalizedText
{
	Q_OBJECT
	Q_PROPERTY(QString m_text READ GetText WRITE SetText NOTIFY textChanged)
	Q_PROPERTY(QString m_locale READ GetLocale WRITE SetLocale NOTIFY localeChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CLocalizedTextObject(QObject* parent = nullptr);

	QString GetText();
	void SetText(QString v);
	Q_INVOKABLE bool hasText();
	QString GetLocale();
	void SetLocale(QString v);
	Q_INVOKABLE bool hasLocale();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void textChanged();
	void localeChanged();
	void finished();

protected:
};


class CLocalizedTextObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CLocalizedText::V1_0, sdl::complextest::ComplexUnion2::CLocalizedTextObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CLocalizedText::V1_0, sdl::complextest::ComplexUnion2::CLocalizedTextObject> BaseClass;

	CLocalizedTextObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CLocalizedTextObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CLocalizedTextObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CLocalizedTextObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CLocalizedTextObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CLocalizedTextObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CLocalizedTextObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CLocalizedTextObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CLocalizedTextObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CLocalizedTextObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CLocalizedTextObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CLocalizedTextObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CLocalizedTextObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CLocalizedTextObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CLocalizedTextObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CLocalizedTextObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CLocalizedTextObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CLocalizedTextObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CLocalizedTextObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CLocalizedTextObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CLocalizedTextObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CLocalizedTextObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CLocalizedTextObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CLocalizedTextObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_text"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).text.value());
		}
		if (nameId == "m_locale"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).locale.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CCoordinatesObjectList;

class CCoordinatesObject: public ::imtbase::CItemModelBase, public CCoordinates
{
	Q_OBJECT
	Q_PROPERTY(double m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(double m_y READ GetY WRITE SetY NOTIFY yChanged)
	Q_PROPERTY(double m_z READ GetZ WRITE SetZ NOTIFY zChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCoordinatesObject(QObject* parent = nullptr);

	double GetX();
	void SetX(double v);
	Q_INVOKABLE bool hasX();
	double GetY();
	void SetY(double v);
	Q_INVOKABLE bool hasY();
	double GetZ();
	void SetZ(double v);
	Q_INVOKABLE bool hasZ();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void xChanged();
	void yChanged();
	void zChanged();
	void finished();

protected:
};


class CCoordinatesObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCoordinates::V1_0, sdl::complextest::ComplexUnion2::CCoordinatesObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCoordinates::V1_0, sdl::complextest::ComplexUnion2::CCoordinatesObject> BaseClass;

	CCoordinatesObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCoordinatesObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCoordinatesObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CCoordinatesObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CCoordinatesObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCoordinatesObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCoordinatesObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCoordinatesObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CCoordinatesObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCoordinatesObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCoordinatesObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCoordinatesObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCoordinatesObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCoordinatesObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCoordinatesObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCoordinatesObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCoordinatesObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CCoordinatesObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCoordinatesObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCoordinatesObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCoordinatesObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCoordinatesObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCoordinatesObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CCoordinatesObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_x"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).x.value());
		}
		if (nameId == "m_y"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).y.value());
		}
		if (nameId == "m_z"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).z.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CExtendedMetaDataObjectList;

class CExtendedMetaDataObject: public ::imtbase::CItemModelBase, public CExtendedMetaData
{
	Q_OBJECT
	Q_PROPERTY(QString m_key READ GetKey WRITE SetKey NOTIFY keyChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CExtendedMetaDataObject(QObject* parent = nullptr);

	QString GetKey();
	void SetKey(QString v);
	Q_INVOKABLE bool hasKey();
	sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* GetValue();
	void SetValue(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* v);
	Q_INVOKABLE bool hasValue();
	Q_INVOKABLE void createValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void keyChanged();
	void valueChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* m_valueQObjectPtr;
};


class CExtendedMetaDataObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CExtendedMetaData::V1_0, sdl::complextest::ComplexUnion2::CExtendedMetaDataObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CExtendedMetaData::V1_0, sdl::complextest::ComplexUnion2::CExtendedMetaDataObject> BaseClass;

	CExtendedMetaDataObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CExtendedMetaDataObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_key"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).key.value());
		}
		if (nameId == "m_value"){
			sdl::complextest::ComplexUnion2::CExtendedMetaDataObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetValue());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CResultMetaDataObjectList;

class CResultMetaDataObject: public ::imtbase::CItemModelBase, public CResultMetaData
{
	Q_OBJECT
	Q_PROPERTY(QString m_resultId READ GetResultId WRITE SetResultId NOTIFY resultIdChanged)
	Q_PROPERTY(QString m_creationTime READ GetCreationTime WRITE SetCreationTime NOTIFY creationTimeChanged)
	Q_PROPERTY(QString m_partId READ GetPartId WRITE SetPartId NOTIFY partIdChanged)
	Q_PROPERTY(QString m_productId READ GetProductId WRITE SetProductId NOTIFY productIdChanged)
	Q_PROPERTY(QString m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(int m_classification READ GetClassification WRITE SetClassification NOTIFY classificationChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CLocalizedTextObject* m_description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* m_extendedMetaData READ GetExtendedMetaData WRITE SetExtendedMetaData NOTIFY extendedMetaDataChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CResultMetaDataObject(QObject* parent = nullptr);

	QString GetResultId();
	void SetResultId(QString v);
	Q_INVOKABLE bool hasResultId();
	QString GetCreationTime();
	void SetCreationTime(QString v);
	Q_INVOKABLE bool hasCreationTime();
	QString GetPartId();
	void SetPartId(QString v);
	Q_INVOKABLE bool hasPartId();
	QString GetProductId();
	void SetProductId(QString v);
	Q_INVOKABLE bool hasProductId();
	QString GetName();
	void SetName(QString v);
	Q_INVOKABLE bool hasName();
	int GetClassification();
	void SetClassification(int v);
	Q_INVOKABLE bool hasClassification();
	sdl::complextest::ComplexUnion2::CLocalizedTextObject* GetDescription();
	void SetDescription(sdl::complextest::ComplexUnion2::CLocalizedTextObject* v);
	Q_INVOKABLE bool hasDescription();
	Q_INVOKABLE void createDescription();
	sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* GetExtendedMetaData();
	void SetExtendedMetaData(sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* v);
	Q_INVOKABLE bool hasExtendedMetaData();
	Q_INVOKABLE void createExtendedMetaData();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
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
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CLocalizedTextObject* m_descriptionQObjectPtr;
	sdl::complextest::ComplexUnion2::CExtendedMetaDataObjectList* m_extendedMetaDataQObjectPtr;
};


class CResultMetaDataObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CResultMetaData::V1_0, sdl::complextest::ComplexUnion2::CResultMetaDataObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CResultMetaData::V1_0, sdl::complextest::ComplexUnion2::CResultMetaDataObject> BaseClass;

	CResultMetaDataObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CResultMetaDataObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CResultMetaDataObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CResultMetaDataObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CResultMetaDataObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CResultMetaDataObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CResultMetaDataObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CResultMetaDataObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CResultMetaDataObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CResultMetaDataObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CResultMetaDataObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CResultMetaDataObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CResultMetaDataObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CResultMetaDataObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CResultMetaDataObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CResultMetaDataObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CResultMetaDataObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CResultMetaDataObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CResultMetaDataObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CResultMetaDataObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CResultMetaDataObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CResultMetaDataObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CResultMetaDataObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CResultMetaDataObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_resultId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).resultId.value());
		}
		if (nameId == "m_creationTime"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).creationTime.value());
		}
		if (nameId == "m_partId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).partId.value());
		}
		if (nameId == "m_productId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).productId.value());
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).name.value());
		}
		if (nameId == "m_classification"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).classification.value());
		}
		if (nameId == "m_description"){
			sdl::complextest::ComplexUnion2::CResultMetaDataObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetDescription());
		}
		if (nameId == "m_extendedMetaData"){
			sdl::complextest::ComplexUnion2::CResultMetaDataObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetExtendedMetaData());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class COverallResultValuesObjectList;

class COverallResultValuesObject: public ::imtbase::CItemModelBase, public COverallResultValues
{
	Q_OBJECT
	Q_PROPERTY(QString m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QString m_valueId READ GetValueId WRITE SetValueId NOTIFY valueIdChanged)
	Q_PROPERTY(double m_measuredValue READ GetMeasuredValue WRITE SetMeasuredValue NOTIFY measuredValueChanged)
	Q_PROPERTY(double m_lowLimit READ GetLowLimit WRITE SetLowLimit NOTIFY lowLimitChanged)
	Q_PROPERTY(double m_highLimit READ GetHighLimit WRITE SetHighLimit NOTIFY highLimitChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COverallResultValuesObject(QObject* parent = nullptr);

	QString GetName();
	void SetName(QString v);
	Q_INVOKABLE bool hasName();
	QString GetValueId();
	void SetValueId(QString v);
	Q_INVOKABLE bool hasValueId();
	double GetMeasuredValue();
	void SetMeasuredValue(double v);
	Q_INVOKABLE bool hasMeasuredValue();
	double GetLowLimit();
	void SetLowLimit(double v);
	Q_INVOKABLE bool hasLowLimit();
	double GetHighLimit();
	void SetHighLimit(double v);
	Q_INVOKABLE bool hasHighLimit();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void nameChanged();
	void valueIdChanged();
	void measuredValueChanged();
	void lowLimitChanged();
	void highLimitChanged();
	void finished();

protected:
};


class COverallResultValuesObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::COverallResultValues::V1_0, sdl::complextest::ComplexUnion2::COverallResultValuesObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::COverallResultValues::V1_0, sdl::complextest::ComplexUnion2::COverallResultValuesObject> BaseClass;

	COverallResultValuesObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::COverallResultValuesObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::COverallResultValuesObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::COverallResultValuesObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::COverallResultValuesObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::COverallResultValuesObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::COverallResultValuesObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::COverallResultValuesObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::COverallResultValuesObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::COverallResultValuesObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::COverallResultValuesObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::COverallResultValuesObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::COverallResultValuesObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::COverallResultValuesObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::COverallResultValuesObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::COverallResultValuesObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::COverallResultValuesObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::COverallResultValuesObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::COverallResultValuesObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::COverallResultValuesObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::COverallResultValuesObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).name.value());
		}
		if (nameId == "m_valueId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).valueId.value());
		}
		if (nameId == "m_measuredValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).measuredValue.value());
		}
		if (nameId == "m_lowLimit"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).lowLimit.value());
		}
		if (nameId == "m_highLimit"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).highLimit.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CUnionTestingTypeObjectList;

class CUnionTestingTypeObject: public ::imtbase::CItemModelBase, public CUnionTestingType
{
	Q_OBJECT
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionSimpleTypeObject* m_simpleUnion READ GetSimpleUnion WRITE SetSimpleUnion NOTIFY simpleUnionChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObject* m_complexUnion READ GetComplexUnion WRITE SetComplexUnion NOTIFY complexUnionChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* m_mixedUnion READ GetMixedUnion WRITE SetMixedUnion NOTIFY mixedUnionChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CUnionTestingTypeObject(QObject* parent = nullptr);

	sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionSimpleTypeObject* GetSimpleUnion();
	void SetSimpleUnion(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionSimpleTypeObject* v);
	Q_INVOKABLE bool hasSimpleUnion();
	Q_INVOKABLE void createSimpleUnion();
	sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObject* GetComplexUnion();
	void SetComplexUnion(sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObject* v);
	Q_INVOKABLE bool hasComplexUnion();
	Q_INVOKABLE void createComplexUnion();
	sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* GetMixedUnion();
	void SetMixedUnion(sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* v);
	Q_INVOKABLE bool hasMixedUnion();
	Q_INVOKABLE void createMixedUnion();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void simpleUnionChanged();
	void complexUnionChanged();
	void mixedUnionChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionSimpleTypeObject* m_simpleUnionQObjectPtr;
	sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObject* m_complexUnionQObjectPtr;
	sdl::complextest::ComplexUnion2::CExtendedMetaDataUnionTypeObject* m_mixedUnionQObjectPtr;
};


class CUnionTestingTypeObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CUnionTestingType::V1_0, sdl::complextest::ComplexUnion2::CUnionTestingTypeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CUnionTestingType::V1_0, sdl::complextest::ComplexUnion2::CUnionTestingTypeObject> BaseClass;

	CUnionTestingTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CUnionTestingTypeObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CUnionTestingTypeObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CUnionTestingTypeObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CUnionTestingTypeObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CUnionTestingTypeObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_simpleUnion"){
			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetSimpleUnion());
		}
		if (nameId == "m_complexUnion"){
			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetComplexUnion());
		}
		if (nameId == "m_mixedUnion"){
			sdl::complextest::ComplexUnion2::CUnionTestingTypeObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetMixedUnion());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CCDMResultVarRecursiveObjectList;

class CCDMResultVarRecursiveObject: public ::imtbase::CItemModelBase, public CCDMResultVarRecursive
{
	Q_OBJECT
	Q_PROPERTY(QString m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QString m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CResultMetaDataObject* m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultVarRecursiveObject(QObject* parent = nullptr);

	QString GetDataModelTitle();
	void SetDataModelTitle(QString v);
	Q_INVOKABLE bool hasDataModelTitle();
	QString GetDataModelVersion();
	void SetDataModelVersion(QString v);
	Q_INVOKABLE bool hasDataModelVersion();
	sdl::complextest::ComplexUnion2::CResultMetaDataObject* GetResultMetaData();
	void SetResultMetaData(sdl::complextest::ComplexUnion2::CResultMetaDataObject* v);
	Q_INVOKABLE bool hasResultMetaData();
	Q_INVOKABLE void createResultMetaData();
	sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* GetResultContent();
	void SetResultContent(sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* v);
	Q_INVOKABLE bool hasResultContent();
	Q_INVOKABLE void createResultContent();
	sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* GetOverallResultValues();
	void SetOverallResultValues(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* v);
	Q_INVOKABLE bool hasOverallResultValues();
	Q_INVOKABLE void createOverallResultValues();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataModelTitleChanged();
	void dataModelVersionChanged();
	void resultMetaDataChanged();
	void resultContentChanged();
	void overallResultValuesChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CResultMetaDataObject* m_resultMetaDataQObjectPtr;
	sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* m_resultContentQObjectPtr;
	sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* m_overallResultValuesQObjectPtr;
};


class CCDMResultVarRecursiveObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarRecursive::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarRecursive::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject> BaseClass;

	CCDMResultVarRecursiveObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_dataModelTitle"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dataModelTitle.value());
		}
		if (nameId == "m_dataModelVersion"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dataModelVersion.value());
		}
		if (nameId == "m_resultMetaData"){
			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetResultMetaData());
		}
		if (nameId == "m_resultContent"){
			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetResultContent());
		}
		if (nameId == "m_overallResultValues"){
			sdl::complextest::ComplexUnion2::CCDMResultVarRecursiveObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetOverallResultValues());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CCDMResultVarStringObjectList;

class CCDMResultVarStringObject: public ::imtbase::CItemModelBase, public CCDMResultVarString
{
	Q_OBJECT
	Q_PROPERTY(QString m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QString m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CResultMetaDataObject* m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(QList<QString> m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultVarStringObject(QObject* parent = nullptr);

	QString GetDataModelTitle();
	void SetDataModelTitle(QString v);
	Q_INVOKABLE bool hasDataModelTitle();
	QString GetDataModelVersion();
	void SetDataModelVersion(QString v);
	Q_INVOKABLE bool hasDataModelVersion();
	sdl::complextest::ComplexUnion2::CResultMetaDataObject* GetResultMetaData();
	void SetResultMetaData(sdl::complextest::ComplexUnion2::CResultMetaDataObject* v);
	Q_INVOKABLE bool hasResultMetaData();
	Q_INVOKABLE void createResultMetaData();
	QList<QString> GetResultContent();
	void SetResultContent(QList<QString> v);
	Q_INVOKABLE bool hasResultContent();
	sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* GetOverallResultValues();
	void SetOverallResultValues(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* v);
	Q_INVOKABLE bool hasOverallResultValues();
	Q_INVOKABLE void createOverallResultValues();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataModelTitleChanged();
	void dataModelVersionChanged();
	void resultMetaDataChanged();
	void resultContentChanged();
	void overallResultValuesChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CResultMetaDataObject* m_resultMetaDataQObjectPtr;
	sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* m_overallResultValuesQObjectPtr;
};


class CCDMResultVarStringObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarString::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarStringObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResultVarString::V1_0, sdl::complextest::ComplexUnion2::CCDMResultVarStringObject> BaseClass;

	CCDMResultVarStringObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCDMResultVarStringObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CCDMResultVarStringObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CCDMResultVarStringObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCDMResultVarStringObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCDMResultVarStringObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_dataModelTitle"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dataModelTitle.value());
		}
		if (nameId == "m_dataModelVersion"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dataModelVersion.value());
		}
		if (nameId == "m_resultMetaData"){
			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetResultMetaData());
		}
		if (nameId == "m_resultContent"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).resultContent.value());
		}
		if (nameId == "m_overallResultValues"){
			sdl::complextest::ComplexUnion2::CCDMResultVarStringObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetOverallResultValues());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CCDMResultObjectList;

class CCDMResultObject: public ::imtbase::CItemModelBase, public CCDMResult
{
	Q_OBJECT
	Q_PROPERTY(QString m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QString m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CResultMetaDataObject* m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultObject(QObject* parent = nullptr);

	QString GetDataModelTitle();
	void SetDataModelTitle(QString v);
	Q_INVOKABLE bool hasDataModelTitle();
	QString GetDataModelVersion();
	void SetDataModelVersion(QString v);
	Q_INVOKABLE bool hasDataModelVersion();
	sdl::complextest::ComplexUnion2::CResultMetaDataObject* GetResultMetaData();
	void SetResultMetaData(sdl::complextest::ComplexUnion2::CResultMetaDataObject* v);
	Q_INVOKABLE bool hasResultMetaData();
	Q_INVOKABLE void createResultMetaData();
	sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* GetResultContent();
	void SetResultContent(sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* v);
	Q_INVOKABLE bool hasResultContent();
	Q_INVOKABLE void createResultContent();
	sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* GetOverallResultValues();
	void SetOverallResultValues(sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* v);
	Q_INVOKABLE bool hasOverallResultValues();
	Q_INVOKABLE void createOverallResultValues();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataModelTitleChanged();
	void dataModelVersionChanged();
	void resultMetaDataChanged();
	void resultContentChanged();
	void overallResultValuesChanged();
	void finished();

protected:
	sdl::complextest::ComplexUnion2::CResultMetaDataObject* m_resultMetaDataQObjectPtr;
	sdl::complextest::ComplexUnion2::CCDMResultUnionTypeObjectList* m_resultContentQObjectPtr;
	sdl::complextest::ComplexUnion2::COverallResultValuesObjectList* m_overallResultValuesQObjectPtr;
};


class CCDMResultObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResult::V1_0, sdl::complextest::ComplexUnion2::CCDMResultObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CCDMResult::V1_0, sdl::complextest::ComplexUnion2::CCDMResultObject> BaseClass;

	CCDMResultObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CCDMResultObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CCDMResultObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CCDMResultObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CCDMResultObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CCDMResultObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CCDMResultObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CCDMResultObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CCDMResultObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CCDMResultObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CCDMResultObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CCDMResultObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCDMResultObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCDMResultObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CCDMResultObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CCDMResultObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CCDMResultObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CCDMResultObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_dataModelTitle"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dataModelTitle.value());
		}
		if (nameId == "m_dataModelVersion"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dataModelVersion.value());
		}
		if (nameId == "m_resultMetaData"){
			sdl::complextest::ComplexUnion2::CCDMResultObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetResultMetaData());
		}
		if (nameId == "m_resultContent"){
			sdl::complextest::ComplexUnion2::CCDMResultObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetResultContent());
		}
		if (nameId == "m_overallResultValues"){
			sdl::complextest::ComplexUnion2::CCDMResultObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetOverallResultValues());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CPointsInCoordinateFormatObjectList;

class CPointsInCoordinateFormatObject: public ::imtbase::CItemModelBase, public CPointsInCoordinateFormat
{
	Q_OBJECT
	Q_PROPERTY(bool m_pointAsCoordinates READ GetPointAsCoordinates WRITE SetPointAsCoordinates NOTIFY pointAsCoordinatesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPointsInCoordinateFormatObject(QObject* parent = nullptr);

	bool GetPointAsCoordinates();
	void SetPointAsCoordinates(bool v);
	Q_INVOKABLE bool hasPointAsCoordinates();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void pointAsCoordinatesChanged();
	void finished();

protected:
};


class CPointsInCoordinateFormatObjectList: public ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormat::V1_0, sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormat::V1_0, sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject> BaseClass;

	CPointsInCoordinateFormatObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObjectList* copyMe(){
		sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObjectList* objectListPtr = new sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject>()){
				return nullptr;
			}

			sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* itemObjectPtr = item.value<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject>()){
				return false;
			}

			sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* selfItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* otherItemObjectPtr = selfItem.value<sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::complextest::ComplexUnion2::CPointsInCoordinateFormatObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_pointAsCoordinates"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).pointAsCoordinates.value());
		}
		return QVariant();
	}
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

class CExtendedMetaDataUnionTypeObject: public ::imtbase::CItemModelBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

public:
	typedef ::imtbase::CItemModelBase BaseClass;

	CExtendedMetaDataUnionTypeObject(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE QString GetType() const{
		return m_type;
	}

	Q_INVOKABLE void SetValue(const QVariant& value){
		if (value.canConvert<CCoordinates>()){
			m_type = "Coordinates";
		}

		if (value.canConvert<double>()){
			m_type = "Float";
		}

		if (value.canConvert<bool>()){
			m_type = "Boolean";
		}

		if (value.canConvert<QString>()){
			m_type = "String";
		}

		m_value = value;
	}

	Q_INVOKABLE QVariant GetValue(){
		return QVariant();
	}

signals:
	void typeChanged();
	void valueChanged();

public:
	istd::TSharedNullable<std::shared_ptr<ExtendedMetaDataUnionType>> Version_1_0;
	QVariant m_value;
	QString m_type;
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

class CExtendedMetaDataUnionSimpleTypeObject: public ::imtbase::CItemModelBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

public:
	typedef ::imtbase::CItemModelBase BaseClass;

	CExtendedMetaDataUnionSimpleTypeObject(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE QString GetType() const{
		return m_type;
	}

	Q_INVOKABLE void SetValue(const QVariant& value){
		if (value.canConvert<double>()){
			m_type = "Float";
		}

		if (value.canConvert<bool>()){
			m_type = "Boolean";
		}

		if (value.canConvert<QString>()){
			m_type = "String";
		}

		m_value = value;
	}

	Q_INVOKABLE QVariant GetValue(){
		return QVariant();
	}

signals:
	void typeChanged();
	void valueChanged();

public:
	istd::TSharedNullable<std::shared_ptr<ExtendedMetaDataUnionSimpleType>> Version_1_0;
	QVariant m_value;
	QString m_type;
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

class CCDMResultUnionTypeObject: public ::imtbase::CItemModelBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

public:
	typedef ::imtbase::CItemModelBase BaseClass;

	CCDMResultUnionTypeObject(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE QString GetType() const{
		return m_type;
	}

	Q_INVOKABLE void SetValue(const QVariant& value){
		if (value.canConvert<CCDMResultVarString>()){
			m_type = "CDMResultVarString";
		}

		if (value.canConvert<CCDMResultVarRecursive>()){
			m_type = "CDMResultVarRecursive";
		}

		m_value = value;
	}

	Q_INVOKABLE QVariant GetValue(){
		return QVariant();
	}

signals:
	void typeChanged();
	void valueChanged();

public:
	istd::TSharedNullable<std::shared_ptr<CDMResultUnionType>> Version_1_0;
	QVariant m_value;
	QString m_type;
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
