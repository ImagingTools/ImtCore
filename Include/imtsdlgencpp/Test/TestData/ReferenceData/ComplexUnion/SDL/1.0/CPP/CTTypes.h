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


namespace sdl::complextest::CTTypes
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
	QString GetNONE() { return "NONE"; }
	QString GetOK() { return "OK"; }
	QString GetWARNING() { return "WARNING"; }
	QString GetNOK() { return "NOK"; }
	QString GetFAILED() { return "FAILED"; }
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
	QString GetOK() { return "OK"; }
	QString GetTOO_WIDE() { return "TOO_WIDE"; }
	QString GetTOLERANCE() { return "TOLERANCE"; }
	QString GetTOO_NARROW() { return "TOO_NARROW"; }
	QString GetNO_DATA_GAP() { return "NO_DATA_GAP"; }
	QString GetGAP() { return "GAP"; }
	QString GetTOO_HIGH() { return "TOO_HIGH"; }
	QString GetTOO_LOW() { return "TOO_LOW"; }
	QString GetTOO_FEW() { return "TOO_FEW"; }
	QString GetTOO_MUCH() { return "TOO_MUCH"; }
	QString GetWRONG_DOT_DIAMETER() { return "WRONG_DOT_DIAMETER"; }
	QString GetWRONG_DOT_POSITION() { return "WRONG_DOT_POSITION"; }
	QString GetMISSING_DOTS() { return "MISSING_DOTS"; }
	QString GetWRONG_DOT_DISTANCE() { return "WRONG_DOT_DISTANCE"; }
	QString GetAREA_TOO_LARGE() { return "AREA_TOO_LARGE"; }
	QString GetTOTALAREA_TOO_LARGE() { return "TOTALAREA_TOO_LARGE"; }
	QString GetBORDERAREA_TOO_LARGE() { return "BORDERAREA_TOO_LARGE"; }
	QString GetWRONG_DOT_AREA() { return "WRONG_DOT_AREA"; }
	QString GetWRONG_DOT_AREA_ONHEIGHT() { return "WRONG_DOT_AREA_ONHEIGHT"; }
	QString GetWRONG_DOT_VOLUME() { return "WRONG_DOT_VOLUME"; }
	QString GetWRONG_DOT_HEIGHT() { return "WRONG_DOT_HEIGHT"; }
	QString GetCONTOUR_MEASUREMENT_ERROR() { return "CONTOUR_MEASUREMENT_ERROR"; }
	QString GetTOO_BRIGHT() { return "TOO_BRIGHT"; }
	QString GetTOO_DARK() { return "TOO_DARK"; }
	QString GetCOUNT_ERROR() { return "COUNT_ERROR"; }
	QString GetZ_DISTANCE_HIGH() { return "Z_DISTANCE_HIGH"; }
	QString GetZ_DISTANCE_LOW() { return "Z_DISTANCE_LOW"; }
	QString GetNOT_SYMMETRICAL() { return "NOT_SYMMETRICAL"; }
	QString GetREFERENCE_3D_ERROR() { return "REFERENCE_3D_ERROR"; }
	QString GetCOLOR_ERROR() { return "COLOR_ERROR"; }
	QString GetPATTERN_COUNT() { return "PATTERN_COUNT"; }
	QString GetPATTERN_POSITION() { return "PATTERN_POSITION"; }
	QString GetPATTERN_ROTATION() { return "PATTERN_ROTATION"; }
	QString GetCENTERLINE() { return "CENTERLINE"; }
	QString GetCALIB_ERROR_COMP() { return "CALIB_ERROR_COMP"; }
	QString GetCALIB_INVALID_COMP() { return "CALIB_INVALID_COMP"; }
	QString GetCALIB_NOZZLE_MOVED() { return "CALIB_NOZZLE_MOVED"; }
	QString GetCONTOUR_NOT_FOUND() { return "CONTOUR_NOT_FOUND"; }
	QString GetCONTOUR_TOO_WIDE() { return "CONTOUR_TOO_WIDE"; }
	QString GetCONTOUR_TOO_NARROW() { return "CONTOUR_TOO_NARROW"; }
	QString GetCONTOUR_GLUE_MOVED() { return "CONTOUR_GLUE_MOVED"; }
	QString GetCONTOUR_INVALID_POLY() { return "CONTOUR_INVALID_POLY"; }
	QString GetREFERENCE_NOT_FOUND() { return "REFERENCE_NOT_FOUND"; }
	QString GetREFERENCE_INVALID_REF() { return "REFERENCE_INVALID_REF"; }
	QString GetREFERENCE_POS_MOVED() { return "REFERENCE_POS_MOVED"; }
	QString GetREFERENCE_DIST_MOVED() { return "REFERENCE_DIST_MOVED"; }
	QString GetAREA_NOT_FOUND() { return "AREA_NOT_FOUND"; }
	QString GetAREA_GLUE_FOUND() { return "AREA_GLUE_FOUND"; }
	QString GetAREA_GLUE_MOVED() { return "AREA_GLUE_MOVED"; }
	QString GetAREA_GAP_LENGTH() { return "AREA_GAP_LENGTH"; }
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
	QString GetNONE() { return "NONE"; }
	QString GetWIDTH() { return "WIDTH"; }
	QString GetBRIGHTNESS() { return "BRIGHTNESS"; }
	QString GetDISTANCE() { return "DISTANCE"; }
	QString GetDISPLACEMENT() { return "DISPLACEMENT"; }
	QString GetHEIGHT() { return "HEIGHT"; }
	QString GetSYMMETRY() { return "SYMMETRY"; }
	QString GetVOLUME() { return "VOLUME"; }
	QString GetVOLUMERATE() { return "VOLUMERATE"; }
	QString GetLENGTH() { return "LENGTH"; }
	QString GetOUT_OF_BOUND() { return "OUT_OF_BOUND"; }
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
	QString GetNONE() { return "NONE"; }
	QString GetMm() { return "mm"; }
	QString GetMm3() { return "mm3"; }
	QString GetMm3_mm() { return "mm3_mm"; }
	QString GetPixel() { return "pixel"; }
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
	QString GetLINE() { return "LINE"; }
	QString GetPOLYGON() { return "POLYGON"; }
	QString GetCIRCLE() { return "CIRCLE"; }
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


class CPointObjectList;

class CPointObject: public ::imtbase::CItemModelBase, public CPoint
{
	Q_OBJECT
	Q_PROPERTY(double m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(double m_y READ GetY WRITE SetY NOTIFY yChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPointObject(QObject* parent = nullptr);

	double GetX();
	void SetX(double v);
	Q_INVOKABLE bool hasX();
	double GetY();
	void SetY(double v);
	Q_INVOKABLE bool hasY();
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
	void finished();

protected:
};


class CPointObjectList: public ::imtbase::TListModelBase<sdl::complextest::CTTypes::CPoint::V1_0, sdl::complextest::CTTypes::CPointObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::CTTypes::CPoint::V1_0, sdl::complextest::CTTypes::CPointObject> BaseClass;

	CPointObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::complextest::CTTypes::CPointObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::CTTypes::CPointObjectList* copyMe(){
		sdl::complextest::CTTypes::CPointObjectList* objectListPtr = new sdl::complextest::CTTypes::CPointObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::CTTypes::CPointObject>()){
				return nullptr;
			}

			sdl::complextest::CTTypes::CPointObject* itemObjectPtr = item.value<sdl::complextest::CTTypes::CPointObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::CTTypes::CPointObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::complextest::CTTypes::CPointObject>()){
				return nullptr;
			}

			sdl::complextest::CTTypes::CPointObject* itemObjectPtr = item.value<sdl::complextest::CTTypes::CPointObject*>();
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
			if (!item.canConvert<sdl::complextest::CTTypes::CPointObject>()){
				return nullptr;
			}

			sdl::complextest::CTTypes::CPointObject* itemObjectPtr = item.value<sdl::complextest::CTTypes::CPointObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::CTTypes::CPointObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::CTTypes::CPointObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::complextest::CTTypes::CPointObject>()){
				return false;
			}

			sdl::complextest::CTTypes::CPointObject* selfItemObjectPtr = selfItem.value<sdl::complextest::CTTypes::CPointObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::CTTypes::CPointObject* otherItemObjectPtr = selfItem.value<sdl::complextest::CTTypes::CPointObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::CTTypes::CPointObject* item){
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
			sdl::complextest::CTTypes::CPointObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_x"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).X.value());
		}
		if (nameId == "m_y"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Y.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};
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
			static const inline QString RequiredPoints = "RequiredPoints";
			static const inline QString OptionalPoints = "OptionalPoints";
		};

		istd::TSharedNullable<GeometryType> GeometryType;
		istd::TSharedNullable<double> Radius;
		istd::TSharedNullable<QList<CPoint::V1_0>> Points;
		istd::TSharedNullable<QList<CPoint::V1_0>> RequiredPoints;
		istd::TSharedNullable<QList<CPoint::V1_0>> OptionalPoints;

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


class CGeometryObjectList;

class CGeometryObject: public ::imtbase::CItemModelBase, public CGeometry
{
	Q_OBJECT
	Q_PROPERTY(QString m_geometryType READ GetGeometryType WRITE SetGeometryType NOTIFY geometryTypeChanged)
	Q_PROPERTY(double m_radius READ GetRadius WRITE SetRadius NOTIFY radiusChanged)
	Q_PROPERTY(sdl::complextest::CTTypes::CPointObjectList* m_points READ GetPoints WRITE SetPoints NOTIFY pointsChanged)
	Q_PROPERTY(sdl::complextest::CTTypes::CPointObjectList* m_requiredPoints READ GetRequiredPoints WRITE SetRequiredPoints NOTIFY requiredPointsChanged)
	Q_PROPERTY(sdl::complextest::CTTypes::CPointObjectList* m_optionalPoints READ GetOptionalPoints WRITE SetOptionalPoints NOTIFY optionalPointsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CGeometryObject(QObject* parent = nullptr);

	QString GetGeometryType();
	void SetGeometryType(QString v);
	Q_INVOKABLE bool hasGeometryType();
	double GetRadius();
	void SetRadius(double v);
	Q_INVOKABLE bool hasRadius();
	sdl::complextest::CTTypes::CPointObjectList* GetPoints();
	void SetPoints(sdl::complextest::CTTypes::CPointObjectList* v);
	Q_INVOKABLE bool hasPoints();
	Q_INVOKABLE void createPoints();
	sdl::complextest::CTTypes::CPointObjectList* GetRequiredPoints();
	void SetRequiredPoints(sdl::complextest::CTTypes::CPointObjectList* v);
	Q_INVOKABLE bool hasRequiredPoints();
	Q_INVOKABLE void createRequiredPoints();
	sdl::complextest::CTTypes::CPointObjectList* GetOptionalPoints();
	void SetOptionalPoints(sdl::complextest::CTTypes::CPointObjectList* v);
	Q_INVOKABLE bool hasOptionalPoints();
	Q_INVOKABLE void createOptionalPoints();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void geometryTypeChanged();
	void radiusChanged();
	void pointsChanged();
	void requiredPointsChanged();
	void optionalPointsChanged();
	void finished();

protected:
	sdl::complextest::CTTypes::CPointObjectList* m_pointsQObjectPtr;
	sdl::complextest::CTTypes::CPointObjectList* m_requiredPointsQObjectPtr;
	sdl::complextest::CTTypes::CPointObjectList* m_optionalPointsQObjectPtr;
};


class CGeometryObjectList: public ::imtbase::TListModelBase<sdl::complextest::CTTypes::CGeometry::V1_0, sdl::complextest::CTTypes::CGeometryObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::complextest::CTTypes::CGeometry::V1_0, sdl::complextest::CTTypes::CGeometryObject> BaseClass;

	CGeometryObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::complextest::CTTypes::CGeometryObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::complextest::CTTypes::CGeometryObjectList* copyMe(){
		sdl::complextest::CTTypes::CGeometryObjectList* objectListPtr = new sdl::complextest::CTTypes::CGeometryObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::complextest::CTTypes::CGeometryObject>()){
				return nullptr;
			}

			sdl::complextest::CTTypes::CGeometryObject* itemObjectPtr = item.value<sdl::complextest::CTTypes::CGeometryObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::complextest::CTTypes::CGeometryObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::complextest::CTTypes::CGeometryObject>()){
				return nullptr;
			}

			sdl::complextest::CTTypes::CGeometryObject* itemObjectPtr = item.value<sdl::complextest::CTTypes::CGeometryObject*>();
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
			if (!item.canConvert<sdl::complextest::CTTypes::CGeometryObject>()){
				return nullptr;
			}

			sdl::complextest::CTTypes::CGeometryObject* itemObjectPtr = item.value<sdl::complextest::CTTypes::CGeometryObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::complextest::CTTypes::CGeometryObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::complextest::CTTypes::CGeometryObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::complextest::CTTypes::CGeometryObject>()){
				return false;
			}

			sdl::complextest::CTTypes::CGeometryObject* selfItemObjectPtr = selfItem.value<sdl::complextest::CTTypes::CGeometryObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::complextest::CTTypes::CGeometryObject* otherItemObjectPtr = selfItem.value<sdl::complextest::CTTypes::CGeometryObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::complextest::CTTypes::CGeometryObject* item){
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
			sdl::complextest::CTTypes::CGeometryObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_geometryType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).GeometryType.value());
		}
		if (nameId == "m_radius"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Radius.value());
		}
		if (nameId == "m_points"){
			sdl::complextest::CTTypes::CGeometryObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetPoints());
		}
		if (nameId == "m_requiredPoints"){
			sdl::complextest::CTTypes::CGeometryObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetRequiredPoints());
		}
		if (nameId == "m_optionalPoints"){
			sdl::complextest::CTTypes::CGeometryObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetOptionalPoints());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};
#ifdef QT_QML_LIB
static void RegisterQmlTypes()
{
	qmlRegisterType<CPointObject>("complextestCTTypesSdl", 1, 0, "Point");
	qmlRegisterType<CGeometryObject>("complextestCTTypesSdl", 1, 0, "Geometry");
	qmlRegisterSingletonType<EnumStatusCode>("complextestCTTypesSdl", 1, 0, "StatusCode", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumStatusCode *enumType = new EnumStatusCode();
		return enumType;
	});
	qmlRegisterSingletonType<EnumErrorCode>("complextestCTTypesSdl", 1, 0, "ErrorCode", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumErrorCode *enumType = new EnumErrorCode();
		return enumType;
	});
	qmlRegisterSingletonType<EnumMeasurementType>("complextestCTTypesSdl", 1, 0, "MeasurementType", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumMeasurementType *enumType = new EnumMeasurementType();
		return enumType;
	});
	qmlRegisterSingletonType<EnumMeasurementUnit>("complextestCTTypesSdl", 1, 0, "MeasurementUnit", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumMeasurementUnit *enumType = new EnumMeasurementUnit();
		return enumType;
	});
	qmlRegisterSingletonType<EnumGeometryType>("complextestCTTypesSdl", 1, 0, "GeometryType", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumGeometryType *enumType = new EnumGeometryType();
		return enumType;
	});
}
#endif

} // namespace sdl::complextest::CTTypes


Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CPoint::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CPoint);
Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CGeometry::V1_0);
Q_DECLARE_METATYPE(sdl::complextest::CTTypes::CGeometry);
