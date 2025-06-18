/// \file StatusCode.h

#pragma once


namespace sdl::complextest::CTTypes
{


enum class StatusCode {
	NONE,
	OK,
	WARNING,
	NOK,
	FAILED,
};

} // namespace sdl::complextest::CTTypes




/// \file ErrorCode.h

#pragma once


namespace sdl::complextest::CTTypes
{


enum class ErrorCode {
	OK,
	TOO_WIDE,
	TOLERANCE,
	TOO_NARROW,
	NO_DATA_GAP,
	GAP,
	TOO_HIGH,
	TOO_LOW,
	TOO_FEW,
	TOO_MUCH,
	WRONG_DOT_DIAMETER,
	WRONG_DOT_POSITION,
	MISSING_DOTS,
	WRONG_DOT_DISTANCE,
	AREA_TOO_LARGE,
	TOTALAREA_TOO_LARGE,
	BORDERAREA_TOO_LARGE,
	WRONG_DOT_AREA,
	WRONG_DOT_AREA_ONHEIGHT,
	WRONG_DOT_VOLUME,
	WRONG_DOT_HEIGHT,
	CONTOUR_MEASUREMENT_ERROR,
	TOO_BRIGHT,
	TOO_DARK,
	COUNT_ERROR,
	Z_DISTANCE_HIGH,
	Z_DISTANCE_LOW,
	NOT_SYMMETRICAL,
	REFERENCE_3D_ERROR,
	COLOR_ERROR,
	PATTERN_COUNT,
	PATTERN_POSITION,
	PATTERN_ROTATION,
	CENTERLINE,
	CALIB_ERROR_COMP,
	CALIB_INVALID_COMP,
	CALIB_NOZZLE_MOVED,
	CONTOUR_NOT_FOUND,
	CONTOUR_TOO_WIDE,
	CONTOUR_TOO_NARROW,
	CONTOUR_GLUE_MOVED,
	CONTOUR_INVALID_POLY,
	REFERENCE_NOT_FOUND,
	REFERENCE_INVALID_REF,
	REFERENCE_POS_MOVED,
	REFERENCE_DIST_MOVED,
	AREA_NOT_FOUND,
	AREA_GLUE_FOUND,
	AREA_GLUE_MOVED,
	AREA_GAP_LENGTH,
};

} // namespace sdl::complextest::CTTypes




/// \file MeasurementType.h

#pragma once


namespace sdl::complextest::CTTypes
{


enum class MeasurementType {
	NONE,
	WIDTH,
	BRIGHTNESS,
	DISTANCE,
	DISPLACEMENT,
	HEIGHT,
	SYMMETRY,
	VOLUME,
	VOLUMERATE,
	LENGTH,
	OUT_OF_BOUND,
};

} // namespace sdl::complextest::CTTypes




/// \file MeasurementUnit.h

#pragma once


namespace sdl::complextest::CTTypes
{


enum class MeasurementUnit {
	NONE,
	mm,
	mm3,
	mm3_mm,
	pixel,
};

} // namespace sdl::complextest::CTTypes




/// \file GeometryType.h

#pragma once


namespace sdl::complextest::CTTypes
{


enum class GeometryType {
	LINE,
	POLYGON,
	CIRCLE,
};

} // namespace sdl::complextest::CTTypes




/// \file CPoint.h

#pragma once


// C/C++ includes
#include <optional>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>



namespace sdl::complextest::CTTypes
{


class CPoint
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
		struct PointFields
		{
			static const inline QString X = "X";
			static const inline QString Y = "Y";
		};

		std::optional<double> X;
		std::optional<double> Y;

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
	std::optional<V1_0> Version_1_0;

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


} // namespace sdl::complextest::CTTypes


Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CPoint::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CPoint);





/// \file CGeometry.h

#pragma once


// C/C++ includes
#include <optional>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>



namespace sdl::complextest::CTTypes
{


class CGeometry
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
		struct GeometryFields
		{
			static const inline QString GeometryType = "GeometryType";
			static const inline QString Radius = "Radius";
			static const inline QString Points = "Points";
		};

		std::optional<GeometryType> GeometryType;
		std::optional<double> Radius;
		std::optional<QList<CPoint::V1_0>> Points;

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
	std::optional<V1_0> Version_1_0;

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


} // namespace sdl::complextest::CTTypes


Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CGeometry::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CGeometry);





