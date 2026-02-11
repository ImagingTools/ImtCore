// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


namespace sdl::complextest::ComplexUnion1
{


Q_NAMESPACE

enum class StatusCode {
	NONE,
	OK,
	WARNING,
	NOK,
	FAILED,
};

Q_ENUM_NS(StatusCode)


class EnumStatusCode: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString NONE READ GetNONE NOTIFY NONEChanged)
	Q_PROPERTY(QString OK READ GetOK NOTIFY OKChanged)
	Q_PROPERTY(QString WARNING READ GetWARNING NOTIFY WARNINGChanged)
	Q_PROPERTY(QString NOK READ GetNOK NOTIFY NOKChanged)
	Q_PROPERTY(QString FAILED READ GetFAILED NOTIFY FAILEDChanged)

protected:
	static QString GetNONE() { return "NONE"; }
	static QString GetOK() { return "OK"; }
	static QString GetWARNING() { return "WARNING"; }
	static QString GetNOK() { return "NOK"; }
	static QString GetFAILED() { return "FAILED"; }

signals:
	void NONEChanged();
	void OKChanged();
	void WARNINGChanged();
	void NOKChanged();
	void FAILEDChanged();
};


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

Q_ENUM_NS(ErrorCode)


class EnumErrorCode: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString OK READ GetOK NOTIFY OKChanged)
	Q_PROPERTY(QString TOO_WIDE READ GetTOO_WIDE NOTIFY TOO_WIDEChanged)
	Q_PROPERTY(QString TOLERANCE READ GetTOLERANCE NOTIFY TOLERANCEChanged)
	Q_PROPERTY(QString TOO_NARROW READ GetTOO_NARROW NOTIFY TOO_NARROWChanged)
	Q_PROPERTY(QString NO_DATA_GAP READ GetNO_DATA_GAP NOTIFY NO_DATA_GAPChanged)
	Q_PROPERTY(QString GAP READ GetGAP NOTIFY GAPChanged)
	Q_PROPERTY(QString TOO_HIGH READ GetTOO_HIGH NOTIFY TOO_HIGHChanged)
	Q_PROPERTY(QString TOO_LOW READ GetTOO_LOW NOTIFY TOO_LOWChanged)
	Q_PROPERTY(QString TOO_FEW READ GetTOO_FEW NOTIFY TOO_FEWChanged)
	Q_PROPERTY(QString TOO_MUCH READ GetTOO_MUCH NOTIFY TOO_MUCHChanged)
	Q_PROPERTY(QString WRONG_DOT_DIAMETER READ GetWRONG_DOT_DIAMETER NOTIFY WRONG_DOT_DIAMETERChanged)
	Q_PROPERTY(QString WRONG_DOT_POSITION READ GetWRONG_DOT_POSITION NOTIFY WRONG_DOT_POSITIONChanged)
	Q_PROPERTY(QString MISSING_DOTS READ GetMISSING_DOTS NOTIFY MISSING_DOTSChanged)
	Q_PROPERTY(QString WRONG_DOT_DISTANCE READ GetWRONG_DOT_DISTANCE NOTIFY WRONG_DOT_DISTANCEChanged)
	Q_PROPERTY(QString AREA_TOO_LARGE READ GetAREA_TOO_LARGE NOTIFY AREA_TOO_LARGEChanged)
	Q_PROPERTY(QString TOTALAREA_TOO_LARGE READ GetTOTALAREA_TOO_LARGE NOTIFY TOTALAREA_TOO_LARGEChanged)
	Q_PROPERTY(QString BORDERAREA_TOO_LARGE READ GetBORDERAREA_TOO_LARGE NOTIFY BORDERAREA_TOO_LARGEChanged)
	Q_PROPERTY(QString WRONG_DOT_AREA READ GetWRONG_DOT_AREA NOTIFY WRONG_DOT_AREAChanged)
	Q_PROPERTY(QString WRONG_DOT_AREA_ONHEIGHT READ GetWRONG_DOT_AREA_ONHEIGHT NOTIFY WRONG_DOT_AREA_ONHEIGHTChanged)
	Q_PROPERTY(QString WRONG_DOT_VOLUME READ GetWRONG_DOT_VOLUME NOTIFY WRONG_DOT_VOLUMEChanged)
	Q_PROPERTY(QString WRONG_DOT_HEIGHT READ GetWRONG_DOT_HEIGHT NOTIFY WRONG_DOT_HEIGHTChanged)
	Q_PROPERTY(QString CONTOUR_MEASUREMENT_ERROR READ GetCONTOUR_MEASUREMENT_ERROR NOTIFY CONTOUR_MEASUREMENT_ERRORChanged)
	Q_PROPERTY(QString TOO_BRIGHT READ GetTOO_BRIGHT NOTIFY TOO_BRIGHTChanged)
	Q_PROPERTY(QString TOO_DARK READ GetTOO_DARK NOTIFY TOO_DARKChanged)
	Q_PROPERTY(QString COUNT_ERROR READ GetCOUNT_ERROR NOTIFY COUNT_ERRORChanged)
	Q_PROPERTY(QString Z_DISTANCE_HIGH READ GetZ_DISTANCE_HIGH NOTIFY Z_DISTANCE_HIGHChanged)
	Q_PROPERTY(QString Z_DISTANCE_LOW READ GetZ_DISTANCE_LOW NOTIFY Z_DISTANCE_LOWChanged)
	Q_PROPERTY(QString NOT_SYMMETRICAL READ GetNOT_SYMMETRICAL NOTIFY NOT_SYMMETRICALChanged)
	Q_PROPERTY(QString REFERENCE_3D_ERROR READ GetREFERENCE_3D_ERROR NOTIFY REFERENCE_3D_ERRORChanged)
	Q_PROPERTY(QString COLOR_ERROR READ GetCOLOR_ERROR NOTIFY COLOR_ERRORChanged)
	Q_PROPERTY(QString PATTERN_COUNT READ GetPATTERN_COUNT NOTIFY PATTERN_COUNTChanged)
	Q_PROPERTY(QString PATTERN_POSITION READ GetPATTERN_POSITION NOTIFY PATTERN_POSITIONChanged)
	Q_PROPERTY(QString PATTERN_ROTATION READ GetPATTERN_ROTATION NOTIFY PATTERN_ROTATIONChanged)
	Q_PROPERTY(QString CENTERLINE READ GetCENTERLINE NOTIFY CENTERLINEChanged)
	Q_PROPERTY(QString CALIB_ERROR_COMP READ GetCALIB_ERROR_COMP NOTIFY CALIB_ERROR_COMPChanged)
	Q_PROPERTY(QString CALIB_INVALID_COMP READ GetCALIB_INVALID_COMP NOTIFY CALIB_INVALID_COMPChanged)
	Q_PROPERTY(QString CALIB_NOZZLE_MOVED READ GetCALIB_NOZZLE_MOVED NOTIFY CALIB_NOZZLE_MOVEDChanged)
	Q_PROPERTY(QString CONTOUR_NOT_FOUND READ GetCONTOUR_NOT_FOUND NOTIFY CONTOUR_NOT_FOUNDChanged)
	Q_PROPERTY(QString CONTOUR_TOO_WIDE READ GetCONTOUR_TOO_WIDE NOTIFY CONTOUR_TOO_WIDEChanged)
	Q_PROPERTY(QString CONTOUR_TOO_NARROW READ GetCONTOUR_TOO_NARROW NOTIFY CONTOUR_TOO_NARROWChanged)
	Q_PROPERTY(QString CONTOUR_GLUE_MOVED READ GetCONTOUR_GLUE_MOVED NOTIFY CONTOUR_GLUE_MOVEDChanged)
	Q_PROPERTY(QString CONTOUR_INVALID_POLY READ GetCONTOUR_INVALID_POLY NOTIFY CONTOUR_INVALID_POLYChanged)
	Q_PROPERTY(QString REFERENCE_NOT_FOUND READ GetREFERENCE_NOT_FOUND NOTIFY REFERENCE_NOT_FOUNDChanged)
	Q_PROPERTY(QString REFERENCE_INVALID_REF READ GetREFERENCE_INVALID_REF NOTIFY REFERENCE_INVALID_REFChanged)
	Q_PROPERTY(QString REFERENCE_POS_MOVED READ GetREFERENCE_POS_MOVED NOTIFY REFERENCE_POS_MOVEDChanged)
	Q_PROPERTY(QString REFERENCE_DIST_MOVED READ GetREFERENCE_DIST_MOVED NOTIFY REFERENCE_DIST_MOVEDChanged)
	Q_PROPERTY(QString AREA_NOT_FOUND READ GetAREA_NOT_FOUND NOTIFY AREA_NOT_FOUNDChanged)
	Q_PROPERTY(QString AREA_GLUE_FOUND READ GetAREA_GLUE_FOUND NOTIFY AREA_GLUE_FOUNDChanged)
	Q_PROPERTY(QString AREA_GLUE_MOVED READ GetAREA_GLUE_MOVED NOTIFY AREA_GLUE_MOVEDChanged)
	Q_PROPERTY(QString AREA_GAP_LENGTH READ GetAREA_GAP_LENGTH NOTIFY AREA_GAP_LENGTHChanged)

protected:
	static QString GetOK() { return "OK"; }
	static QString GetTOO_WIDE() { return "TOO_WIDE"; }
	static QString GetTOLERANCE() { return "TOLERANCE"; }
	static QString GetTOO_NARROW() { return "TOO_NARROW"; }
	static QString GetNO_DATA_GAP() { return "NO_DATA_GAP"; }
	static QString GetGAP() { return "GAP"; }
	static QString GetTOO_HIGH() { return "TOO_HIGH"; }
	static QString GetTOO_LOW() { return "TOO_LOW"; }
	static QString GetTOO_FEW() { return "TOO_FEW"; }
	static QString GetTOO_MUCH() { return "TOO_MUCH"; }
	static QString GetWRONG_DOT_DIAMETER() { return "WRONG_DOT_DIAMETER"; }
	static QString GetWRONG_DOT_POSITION() { return "WRONG_DOT_POSITION"; }
	static QString GetMISSING_DOTS() { return "MISSING_DOTS"; }
	static QString GetWRONG_DOT_DISTANCE() { return "WRONG_DOT_DISTANCE"; }
	static QString GetAREA_TOO_LARGE() { return "AREA_TOO_LARGE"; }
	static QString GetTOTALAREA_TOO_LARGE() { return "TOTALAREA_TOO_LARGE"; }
	static QString GetBORDERAREA_TOO_LARGE() { return "BORDERAREA_TOO_LARGE"; }
	static QString GetWRONG_DOT_AREA() { return "WRONG_DOT_AREA"; }
	static QString GetWRONG_DOT_AREA_ONHEIGHT() { return "WRONG_DOT_AREA_ONHEIGHT"; }
	static QString GetWRONG_DOT_VOLUME() { return "WRONG_DOT_VOLUME"; }
	static QString GetWRONG_DOT_HEIGHT() { return "WRONG_DOT_HEIGHT"; }
	static QString GetCONTOUR_MEASUREMENT_ERROR() { return "CONTOUR_MEASUREMENT_ERROR"; }
	static QString GetTOO_BRIGHT() { return "TOO_BRIGHT"; }
	static QString GetTOO_DARK() { return "TOO_DARK"; }
	static QString GetCOUNT_ERROR() { return "COUNT_ERROR"; }
	static QString GetZ_DISTANCE_HIGH() { return "Z_DISTANCE_HIGH"; }
	static QString GetZ_DISTANCE_LOW() { return "Z_DISTANCE_LOW"; }
	static QString GetNOT_SYMMETRICAL() { return "NOT_SYMMETRICAL"; }
	static QString GetREFERENCE_3D_ERROR() { return "REFERENCE_3D_ERROR"; }
	static QString GetCOLOR_ERROR() { return "COLOR_ERROR"; }
	static QString GetPATTERN_COUNT() { return "PATTERN_COUNT"; }
	static QString GetPATTERN_POSITION() { return "PATTERN_POSITION"; }
	static QString GetPATTERN_ROTATION() { return "PATTERN_ROTATION"; }
	static QString GetCENTERLINE() { return "CENTERLINE"; }
	static QString GetCALIB_ERROR_COMP() { return "CALIB_ERROR_COMP"; }
	static QString GetCALIB_INVALID_COMP() { return "CALIB_INVALID_COMP"; }
	static QString GetCALIB_NOZZLE_MOVED() { return "CALIB_NOZZLE_MOVED"; }
	static QString GetCONTOUR_NOT_FOUND() { return "CONTOUR_NOT_FOUND"; }
	static QString GetCONTOUR_TOO_WIDE() { return "CONTOUR_TOO_WIDE"; }
	static QString GetCONTOUR_TOO_NARROW() { return "CONTOUR_TOO_NARROW"; }
	static QString GetCONTOUR_GLUE_MOVED() { return "CONTOUR_GLUE_MOVED"; }
	static QString GetCONTOUR_INVALID_POLY() { return "CONTOUR_INVALID_POLY"; }
	static QString GetREFERENCE_NOT_FOUND() { return "REFERENCE_NOT_FOUND"; }
	static QString GetREFERENCE_INVALID_REF() { return "REFERENCE_INVALID_REF"; }
	static QString GetREFERENCE_POS_MOVED() { return "REFERENCE_POS_MOVED"; }
	static QString GetREFERENCE_DIST_MOVED() { return "REFERENCE_DIST_MOVED"; }
	static QString GetAREA_NOT_FOUND() { return "AREA_NOT_FOUND"; }
	static QString GetAREA_GLUE_FOUND() { return "AREA_GLUE_FOUND"; }
	static QString GetAREA_GLUE_MOVED() { return "AREA_GLUE_MOVED"; }
	static QString GetAREA_GAP_LENGTH() { return "AREA_GAP_LENGTH"; }

signals:
	void OKChanged();
	void TOO_WIDEChanged();
	void TOLERANCEChanged();
	void TOO_NARROWChanged();
	void NO_DATA_GAPChanged();
	void GAPChanged();
	void TOO_HIGHChanged();
	void TOO_LOWChanged();
	void TOO_FEWChanged();
	void TOO_MUCHChanged();
	void WRONG_DOT_DIAMETERChanged();
	void WRONG_DOT_POSITIONChanged();
	void MISSING_DOTSChanged();
	void WRONG_DOT_DISTANCEChanged();
	void AREA_TOO_LARGEChanged();
	void TOTALAREA_TOO_LARGEChanged();
	void BORDERAREA_TOO_LARGEChanged();
	void WRONG_DOT_AREAChanged();
	void WRONG_DOT_AREA_ONHEIGHTChanged();
	void WRONG_DOT_VOLUMEChanged();
	void WRONG_DOT_HEIGHTChanged();
	void CONTOUR_MEASUREMENT_ERRORChanged();
	void TOO_BRIGHTChanged();
	void TOO_DARKChanged();
	void COUNT_ERRORChanged();
	void Z_DISTANCE_HIGHChanged();
	void Z_DISTANCE_LOWChanged();
	void NOT_SYMMETRICALChanged();
	void REFERENCE_3D_ERRORChanged();
	void COLOR_ERRORChanged();
	void PATTERN_COUNTChanged();
	void PATTERN_POSITIONChanged();
	void PATTERN_ROTATIONChanged();
	void CENTERLINEChanged();
	void CALIB_ERROR_COMPChanged();
	void CALIB_INVALID_COMPChanged();
	void CALIB_NOZZLE_MOVEDChanged();
	void CONTOUR_NOT_FOUNDChanged();
	void CONTOUR_TOO_WIDEChanged();
	void CONTOUR_TOO_NARROWChanged();
	void CONTOUR_GLUE_MOVEDChanged();
	void CONTOUR_INVALID_POLYChanged();
	void REFERENCE_NOT_FOUNDChanged();
	void REFERENCE_INVALID_REFChanged();
	void REFERENCE_POS_MOVEDChanged();
	void REFERENCE_DIST_MOVEDChanged();
	void AREA_NOT_FOUNDChanged();
	void AREA_GLUE_FOUNDChanged();
	void AREA_GLUE_MOVEDChanged();
	void AREA_GAP_LENGTHChanged();
};


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

Q_ENUM_NS(MeasurementType)


class EnumMeasurementType: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString NONE READ GetNONE NOTIFY NONEChanged)
	Q_PROPERTY(QString WIDTH READ GetWIDTH NOTIFY WIDTHChanged)
	Q_PROPERTY(QString BRIGHTNESS READ GetBRIGHTNESS NOTIFY BRIGHTNESSChanged)
	Q_PROPERTY(QString DISTANCE READ GetDISTANCE NOTIFY DISTANCEChanged)
	Q_PROPERTY(QString DISPLACEMENT READ GetDISPLACEMENT NOTIFY DISPLACEMENTChanged)
	Q_PROPERTY(QString HEIGHT READ GetHEIGHT NOTIFY HEIGHTChanged)
	Q_PROPERTY(QString SYMMETRY READ GetSYMMETRY NOTIFY SYMMETRYChanged)
	Q_PROPERTY(QString VOLUME READ GetVOLUME NOTIFY VOLUMEChanged)
	Q_PROPERTY(QString VOLUMERATE READ GetVOLUMERATE NOTIFY VOLUMERATEChanged)
	Q_PROPERTY(QString LENGTH READ GetLENGTH NOTIFY LENGTHChanged)
	Q_PROPERTY(QString OUT_OF_BOUND READ GetOUT_OF_BOUND NOTIFY OUT_OF_BOUNDChanged)

protected:
	static QString GetNONE() { return "NONE"; }
	static QString GetWIDTH() { return "WIDTH"; }
	static QString GetBRIGHTNESS() { return "BRIGHTNESS"; }
	static QString GetDISTANCE() { return "DISTANCE"; }
	static QString GetDISPLACEMENT() { return "DISPLACEMENT"; }
	static QString GetHEIGHT() { return "HEIGHT"; }
	static QString GetSYMMETRY() { return "SYMMETRY"; }
	static QString GetVOLUME() { return "VOLUME"; }
	static QString GetVOLUMERATE() { return "VOLUMERATE"; }
	static QString GetLENGTH() { return "LENGTH"; }
	static QString GetOUT_OF_BOUND() { return "OUT_OF_BOUND"; }

signals:
	void NONEChanged();
	void WIDTHChanged();
	void BRIGHTNESSChanged();
	void DISTANCEChanged();
	void DISPLACEMENTChanged();
	void HEIGHTChanged();
	void SYMMETRYChanged();
	void VOLUMEChanged();
	void VOLUMERATEChanged();
	void LENGTHChanged();
	void OUT_OF_BOUNDChanged();
};


enum class MeasurementUnit {
	NONE,
	mm,
	mm3,
	mm3_mm,
	pixel,
};

Q_ENUM_NS(MeasurementUnit)


class EnumMeasurementUnit: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString NONE READ GetNONE NOTIFY NONEChanged)
	Q_PROPERTY(QString mm READ GetMm NOTIFY MmChanged)
	Q_PROPERTY(QString mm3 READ GetMm3 NOTIFY Mm3Changed)
	Q_PROPERTY(QString mm3_mm READ GetMm3_mm NOTIFY Mm3_mmChanged)
	Q_PROPERTY(QString pixel READ GetPixel NOTIFY PixelChanged)

protected:
	static QString GetNONE() { return "NONE"; }
	static QString GetMm() { return "mm"; }
	static QString GetMm3() { return "mm3"; }
	static QString GetMm3_mm() { return "mm3_mm"; }
	static QString GetPixel() { return "pixel"; }

signals:
	void NONEChanged();
	void MmChanged();
	void Mm3Changed();
	void Mm3_mmChanged();
	void PixelChanged();
};


enum class GeometryType {
	LINE,
	POLYGON,
	CIRCLE,
};

Q_ENUM_NS(GeometryType)


class EnumGeometryType: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString LINE READ GetLINE NOTIFY LINEChanged)
	Q_PROPERTY(QString POLYGON READ GetPOLYGON NOTIFY POLYGONChanged)
	Q_PROPERTY(QString CIRCLE READ GetCIRCLE NOTIFY CIRCLEChanged)

protected:
	static QString GetLINE() { return "LINE"; }
	static QString GetPOLYGON() { return "POLYGON"; }
	static QString GetCIRCLE() { return "CIRCLE"; }

signals:
	void LINEChanged();
	void POLYGONChanged();
	void CIRCLEChanged();
};



class CPoint
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
		struct PointFields
		{
			static const inline QString X = "X";
			static const inline QString Y = "Y";
		};

		istd::TSharedNullable<double> X;
		istd::TSharedNullable<double> Y;

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

class CGeometry
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
		struct GeometryFields
		{
			static const inline QString GeometryType = "GeometryType";
			static const inline QString Radius = "Radius";
			static const inline QString Points = "Points";
			static const inline QString RequiredPoints = "RequiredPoints";
			static const inline QString OptionalPoints = "OptionalPoints";
		};

		istd::TSharedNullable<GeometryType> GeometryType;
		istd::TSharedNullable<double> Radius;
		istd::TSharedNullable<imtsdl::TElementList<CPoint::V1_0>> Points;
		istd::TSharedNullable<imtsdl::TElementList<CPoint::V1_0>> RequiredPoints;
		istd::TSharedNullable<imtsdl::TElementList<CPoint::V1_0>> OptionalPoints;

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




class CPointObjectList;

class CPointObject: public ::imtbase::CItemModelBase, public CPoint
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(QVariant m_y READ GetY WRITE SetY NOTIFY yChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPointObject(QObject* parent = nullptr);

	QVariant GetX();
	void SetX(const QVariant& v);
	Q_INVOKABLE bool hasX();
	QVariant GetY();
	void SetY(const QVariant& v);
	Q_INVOKABLE bool hasY();
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

protected:
};





class CPointObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion1::CPoint::V1_0, sdl::complextest::ComplexUnion1::CPointObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion1::CPoint::V1_0, sdl::complextest::ComplexUnion1::CPointObject> BaseClass;

	explicit CPointObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion1::CPointObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion1::CPointObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion1::CPointObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion1::CPointObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion1::CPointObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CGeometryObjectList;

class CGeometryObject: public ::imtbase::CItemModelBase, public CGeometry
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_geometryType READ GetGeometryType WRITE SetGeometryType NOTIFY geometryTypeChanged)
	Q_PROPERTY(QVariant m_radius READ GetRadius WRITE SetRadius NOTIFY radiusChanged)
	Q_PROPERTY(QVariant m_points READ GetPoints WRITE SetPoints NOTIFY pointsChanged)
	Q_PROPERTY(QVariant m_requiredPoints READ GetRequiredPoints WRITE SetRequiredPoints NOTIFY requiredPointsChanged)
	Q_PROPERTY(QVariant m_optionalPoints READ GetOptionalPoints WRITE SetOptionalPoints NOTIFY optionalPointsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CGeometryObject(QObject* parent = nullptr);

	QVariant GetGeometryType();
	void SetGeometryType(const QVariant& v);
	Q_INVOKABLE bool hasGeometryType();
	QVariant GetRadius();
	void SetRadius(const QVariant& v);
	Q_INVOKABLE bool hasRadius();
	QVariant GetPoints();
	void SetPoints(const QVariant& v);
	Q_INVOKABLE bool hasPoints();
	Q_INVOKABLE void emplacePoints();
	void ResetPoints();
	Q_INVOKABLE QVariant createPointsArrayElement(const QVariant& v);
	QVariant GetRequiredPoints();
	void SetRequiredPoints(const QVariant& v);
	Q_INVOKABLE bool hasRequiredPoints();
	Q_INVOKABLE void emplaceRequiredPoints();
	void ResetRequiredPoints();
	Q_INVOKABLE QVariant createRequiredPointsArrayElement(const QVariant& v);
	QVariant GetOptionalPoints();
	void SetOptionalPoints(const QVariant& v);
	Q_INVOKABLE bool hasOptionalPoints();
	Q_INVOKABLE void emplaceOptionalPoints();
	void ResetOptionalPoints();
	Q_INVOKABLE QVariant createOptionalPointsArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void geometryTypeChanged();
	void radiusChanged();
	void pointsChanged();
	void requiredPointsChanged();
	void optionalPointsChanged();

protected:
	QVariant m_pointsQObjectPtr;
	QVariant m_requiredPointsQObjectPtr;
	QVariant m_optionalPointsQObjectPtr;
};





class CGeometryObjectList: public ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion1::CGeometry::V1_0, sdl::complextest::ComplexUnion1::CGeometryObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::complextest::ComplexUnion1::CGeometry::V1_0, sdl::complextest::ComplexUnion1::CGeometryObject> BaseClass;

	explicit CGeometryObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::complextest::ComplexUnion1::CGeometryObject* item);
	Q_INVOKABLE sdl::complextest::ComplexUnion1::CGeometryObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::complextest::ComplexUnion1::CGeometryObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::ComplexUnion1::CGeometryObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::complextest::ComplexUnion1::CGeometryObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
{
	qmlRegisterType<CPointObject>("complextestComplexUnion1Sdl", 1, 0, "Point");
	qmlRegisterType<CGeometryObject>("complextestComplexUnion1Sdl", 1, 0, "Geometry");
	qmlRegisterSingletonType<EnumStatusCode>("complextestComplexUnion1Sdl", 1, 0, "StatusCode", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumStatusCode *enumType = new EnumStatusCode();
		return enumType;
	});
	qmlRegisterSingletonType<EnumErrorCode>("complextestComplexUnion1Sdl", 1, 0, "ErrorCode", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumErrorCode *enumType = new EnumErrorCode();
		return enumType;
	});
	qmlRegisterSingletonType<EnumMeasurementType>("complextestComplexUnion1Sdl", 1, 0, "MeasurementType", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumMeasurementType *enumType = new EnumMeasurementType();
		return enumType;
	});
	qmlRegisterSingletonType<EnumMeasurementUnit>("complextestComplexUnion1Sdl", 1, 0, "MeasurementUnit", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumMeasurementUnit *enumType = new EnumMeasurementUnit();
		return enumType;
	});
	qmlRegisterSingletonType<EnumGeometryType>("complextestComplexUnion1Sdl", 1, 0, "GeometryType", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumGeometryType *enumType = new EnumGeometryType();
		return enumType;
	});
}
#endif

} // namespace sdl::complextest::ComplexUnion1


Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion1::CPoint::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion1::CPoint);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion1::CGeometry::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::ComplexUnion1::CGeometry);
