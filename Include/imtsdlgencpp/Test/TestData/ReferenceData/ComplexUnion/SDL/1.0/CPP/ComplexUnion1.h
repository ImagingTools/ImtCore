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


namespace sdl::V1_0::complextest
{


class CPoint
{
public:
	struct PointFields
	{
		static const inline QString X = "X";
		static const inline QString Y = "Y";
	};

	istd::TSharedNullable<double> X;
	istd::TSharedNullable<double> Y;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CPoint& other) const;
	[[nodiscard]] bool operator!=(const CPoint& other) const {return !(operator==(other));}
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

class CGeometry
{
public:
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
	istd::TSharedNullable<imtsdl::TElementList<CPoint>> Points;
	istd::TSharedNullable<imtsdl::TElementList<CPoint>> RequiredPoints;
	istd::TSharedNullable<imtsdl::TElementList<CPoint>> OptionalPoints;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CGeometry& other) const;
	[[nodiscard]] bool operator!=(const CGeometry& other) const {return !(operator==(other));}
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





class CPointObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CPoint, sdl::V1_0::complextest::CPointObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CPoint, sdl::V1_0::complextest::CPointObject> BaseClass;

	explicit CPointObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CPointObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CPointObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CPointObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CPointObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CPointObject* item);
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





class CGeometryObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CGeometry, sdl::V1_0::complextest::CGeometryObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CGeometry, sdl::V1_0::complextest::CGeometryObject> BaseClass;

	explicit CGeometryObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CGeometryObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CGeometryObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CGeometryObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CGeometryObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CGeometryObject* item);
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

} // namespace sdl::V1_0::complextest


Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CPoint);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CGeometry);
