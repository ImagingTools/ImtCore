#pragma once

#include "ComplexUnion2.h"

#ifdef QT_QML_LIB
#include <QtQml/QQmlEngine>
#endif // QT_QML_LIB


// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtsdl/TListModelBase.h>

// custom types includes
#if __has_include(<complextestsdl/SDL/1.0/CPP/ComplexUnion1Objects.h>)
#include <complextestsdl/SDL/1.0/CPP/ComplexUnion1Objects.h>
#else
#error The objects header <complextestsdl/SDL/1.0/CPP/ComplexUnion1Objects.h> is not found. The schema, that defines the imported types, is probably generated without objects (NO_OBJECTS). Generate the objects for it or disable them for this schema.
#endif
#if __has_include(<imtbasesdl/SDL/1.0/CPP/ImtBaseTypesObjects.h>)
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypesObjects.h>
#else
#error The objects header <imtbasesdl/SDL/1.0/CPP/ImtBaseTypesObjects.h> is not found. The schema, that defines the imported types, is probably generated without objects (NO_OBJECTS). Generate the objects for it or disable them for this schema.
#endif
#if __has_include(<imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilterObjects.h>)
#include <imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilterObjects.h>
#else
#error The objects header <imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilterObjects.h> is not found. The schema, that defines the imported types, is probably generated without objects (NO_OBJECTS). Generate the objects for it or disable them for this schema.
#endif


namespace sdl::V1_0::complextest
{


class CExtendedMetaDataUnionTypeObject;
class CExtendedMetaDataUnionTypeObjectList;
class CExtendedMetaDataUnionSimpleTypeObject;
class CExtendedMetaDataUnionSimpleTypeObjectList;
class CCDMResultUnionTypeObject;
class CCDMResultUnionTypeObjectList;




class CResultObjectList;

class CResultObject: public ::imtbase::CItemModelBase, public CResult
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
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
	QString Get__Typename() {return QStringLiteral("Result");}
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





class CResultObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CResult, sdl::V1_0::complextest::CResultObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CResult, sdl::V1_0::complextest::CResultObject> BaseClass;

	explicit CResultObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CResultObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CResultObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CResultObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CResultObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CResultObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_areaName READ GetAreaName WRITE SetAreaName NOTIFY areaNameChanged)
	Q_PROPERTY(QVariant m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QVariant m_iconPosition READ GetIconPosition WRITE SetIconPosition NOTIFY iconPositionChanged)
	Q_PROPERTY(QVariant m_errorType READ GetErrorType WRITE SetErrorType NOTIFY errorTypeChanged)
	Q_PROPERTY(QVariant m_results READ GetResults WRITE SetResults NOTIFY resultsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CAreaObject(QObject* parent = nullptr);

	QVariant GetAreaName();
	QString Get__Typename() {return QStringLiteral("Area");}
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





class CAreaObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CArea, sdl::V1_0::complextest::CAreaObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CArea, sdl::V1_0::complextest::CAreaObject> BaseClass;

	explicit CAreaObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CAreaObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CAreaObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CAreaObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CAreaObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CAreaObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_analyzerName READ GetAnalyzerName WRITE SetAnalyzerName NOTIFY analyzerNameChanged)
	Q_PROPERTY(QVariant m_analyzerResult READ GetAnalyzerResult WRITE SetAnalyzerResult NOTIFY analyzerResultChanged)
	Q_PROPERTY(QVariant m_areas READ GetAreas WRITE SetAreas NOTIFY areasChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CAnalyzerObject(QObject* parent = nullptr);

	QVariant GetAnalyzerName();
	QString Get__Typename() {return QStringLiteral("Analyzer");}
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





class CAnalyzerObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CAnalyzer, sdl::V1_0::complextest::CAnalyzerObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CAnalyzer, sdl::V1_0::complextest::CAnalyzerObject> BaseClass;

	explicit CAnalyzerObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CAnalyzerObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CAnalyzerObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CAnalyzerObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CAnalyzerObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CAnalyzerObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_iD READ GetID WRITE SetID NOTIFY iDChanged)
	Q_PROPERTY(QVariant m_typeID READ GetTypeID WRITE SetTypeID NOTIFY typeIDChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QVariant m_analyzers READ GetAnalyzers WRITE SetAnalyzers NOTIFY analyzersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CInspectionObject(QObject* parent = nullptr);

	QVariant GetID();
	QString Get__Typename() {return QStringLiteral("Inspection");}
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





class CInspectionObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CInspection, sdl::V1_0::complextest::CInspectionObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CInspection, sdl::V1_0::complextest::CInspectionObject> BaseClass;

	explicit CInspectionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CInspectionObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CInspectionObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CInspectionObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CInspectionObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CInspectionObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
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
	QString Get__Typename() {return QStringLiteral("ProductOverview");}
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





class CProductOverviewObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CProductOverview, sdl::V1_0::complextest::CProductOverviewObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CProductOverview, sdl::V1_0::complextest::CProductOverviewObject> BaseClass;

	explicit CProductOverviewObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CProductOverviewObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CProductOverviewObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CProductOverviewObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CProductOverviewObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CProductOverviewObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_text READ GetText WRITE SetText NOTIFY textChanged)
	Q_PROPERTY(QVariant m_locale READ GetLocale WRITE SetLocale NOTIFY localeChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CLocalizedTextObject(QObject* parent = nullptr);

	QVariant GetText();
	QString Get__Typename() {return QStringLiteral("LocalizedText");}
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





class CLocalizedTextObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CLocalizedText, sdl::V1_0::complextest::CLocalizedTextObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CLocalizedText, sdl::V1_0::complextest::CLocalizedTextObject> BaseClass;

	explicit CLocalizedTextObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CLocalizedTextObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CLocalizedTextObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CLocalizedTextObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CLocalizedTextObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CLocalizedTextObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(QVariant m_y READ GetY WRITE SetY NOTIFY yChanged)
	Q_PROPERTY(QVariant m_z READ GetZ WRITE SetZ NOTIFY zChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCoordinatesObject(QObject* parent = nullptr);

	QVariant GetX();
	QString Get__Typename() {return QStringLiteral("Coordinates");}
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





class CCoordinatesObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCoordinates, sdl::V1_0::complextest::CCoordinatesObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCoordinates, sdl::V1_0::complextest::CCoordinatesObject> BaseClass;

	explicit CCoordinatesObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CCoordinatesObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CCoordinatesObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CCoordinatesObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CCoordinatesObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CCoordinatesObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_key READ GetKey WRITE SetKey NOTIFY keyChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CExtendedMetaDataObject(QObject* parent = nullptr);

	QVariant GetKey();
	QString Get__Typename() {return QStringLiteral("ExtendedMetaData");}
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





class CExtendedMetaDataObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CExtendedMetaData, sdl::V1_0::complextest::CExtendedMetaDataObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CExtendedMetaData, sdl::V1_0::complextest::CExtendedMetaDataObject> BaseClass;

	explicit CExtendedMetaDataObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CExtendedMetaDataObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CExtendedMetaDataObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CExtendedMetaDataObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CExtendedMetaDataObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
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
	QString Get__Typename() {return QStringLiteral("ResultMetaData");}
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





class CResultMetaDataObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CResultMetaData, sdl::V1_0::complextest::CResultMetaDataObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CResultMetaData, sdl::V1_0::complextest::CResultMetaDataObject> BaseClass;

	explicit CResultMetaDataObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CResultMetaDataObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CResultMetaDataObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CResultMetaDataObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CResultMetaDataObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CResultMetaDataObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_valueId READ GetValueId WRITE SetValueId NOTIFY valueIdChanged)
	Q_PROPERTY(QVariant m_measuredValue READ GetMeasuredValue WRITE SetMeasuredValue NOTIFY measuredValueChanged)
	Q_PROPERTY(QVariant m_lowLimit READ GetLowLimit WRITE SetLowLimit NOTIFY lowLimitChanged)
	Q_PROPERTY(QVariant m_highLimit READ GetHighLimit WRITE SetHighLimit NOTIFY highLimitChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COverallResultValuesObject(QObject* parent = nullptr);

	QVariant GetName();
	QString Get__Typename() {return QStringLiteral("OverallResultValues");}
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





class COverallResultValuesObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::COverallResultValues, sdl::V1_0::complextest::COverallResultValuesObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::COverallResultValues, sdl::V1_0::complextest::COverallResultValuesObject> BaseClass;

	explicit COverallResultValuesObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::COverallResultValuesObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::COverallResultValuesObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::COverallResultValuesObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::COverallResultValuesObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::COverallResultValuesObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_simpleUnion READ GetSimpleUnion WRITE SetSimpleUnion NOTIFY simpleUnionChanged)
	Q_PROPERTY(QVariant m_complexUnion READ GetComplexUnion WRITE SetComplexUnion NOTIFY complexUnionChanged)
	Q_PROPERTY(QVariant m_mixedUnion READ GetMixedUnion WRITE SetMixedUnion NOTIFY mixedUnionChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CUnionTestingTypeObject(QObject* parent = nullptr);

	QVariant GetSimpleUnion();
	QString Get__Typename() {return QStringLiteral("UnionTestingType");}
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





class CUnionTestingTypeObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CUnionTestingType, sdl::V1_0::complextest::CUnionTestingTypeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CUnionTestingType, sdl::V1_0::complextest::CUnionTestingTypeObject> BaseClass;

	explicit CUnionTestingTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CUnionTestingTypeObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CUnionTestingTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CUnionTestingTypeObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CUnionTestingTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CUnionTestingTypeObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QVariant m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(QVariant m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(QVariant m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(QVariant m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultVarRecursiveObject(QObject* parent = nullptr);

	QVariant GetDataModelTitle();
	QString Get__Typename() {return QStringLiteral("CDMResultVarRecursive");}
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





class CCDMResultVarRecursiveObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCDMResultVarRecursive, sdl::V1_0::complextest::CCDMResultVarRecursiveObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCDMResultVarRecursive, sdl::V1_0::complextest::CCDMResultVarRecursiveObject> BaseClass;

	explicit CCDMResultVarRecursiveObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QVariant m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(QVariant m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(QVariant m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(QVariant m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultVarStringObject(QObject* parent = nullptr);

	QVariant GetDataModelTitle();
	QString Get__Typename() {return QStringLiteral("CDMResultVarString");}
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





class CCDMResultVarStringObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCDMResultVarString, sdl::V1_0::complextest::CCDMResultVarStringObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCDMResultVarString, sdl::V1_0::complextest::CCDMResultVarStringObject> BaseClass;

	explicit CCDMResultVarStringObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CCDMResultVarStringObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CCDMResultVarStringObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CCDMResultVarStringObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CCDMResultVarStringObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CCDMResultVarStringObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_dataModelTitle READ GetDataModelTitle WRITE SetDataModelTitle NOTIFY dataModelTitleChanged)
	Q_PROPERTY(QVariant m_dataModelVersion READ GetDataModelVersion WRITE SetDataModelVersion NOTIFY dataModelVersionChanged)
	Q_PROPERTY(QVariant m_resultMetaData READ GetResultMetaData WRITE SetResultMetaData NOTIFY resultMetaDataChanged)
	Q_PROPERTY(QVariant m_resultContent READ GetResultContent WRITE SetResultContent NOTIFY resultContentChanged)
	Q_PROPERTY(QVariant m_overallResultValues READ GetOverallResultValues WRITE SetOverallResultValues NOTIFY overallResultValuesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCDMResultObject(QObject* parent = nullptr);

	QVariant GetDataModelTitle();
	QString Get__Typename() {return QStringLiteral("CDMResult");}
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





class CCDMResultObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCDMResult, sdl::V1_0::complextest::CCDMResultObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCDMResult, sdl::V1_0::complextest::CCDMResultObject> BaseClass;

	explicit CCDMResultObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CCDMResultObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CCDMResultObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CCDMResultObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CCDMResultObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CCDMResultObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_pointAsCoordinates READ GetPointAsCoordinates WRITE SetPointAsCoordinates NOTIFY pointAsCoordinatesChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPointsInCoordinateFormatObject(QObject* parent = nullptr);

	QVariant GetPointAsCoordinates();
	QString Get__Typename() {return QStringLiteral("PointsInCoordinateFormat");}
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





class CPointsInCoordinateFormatObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CPointsInCoordinateFormat, sdl::V1_0::complextest::CPointsInCoordinateFormatObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CPointsInCoordinateFormat, sdl::V1_0::complextest::CPointsInCoordinateFormatObject> BaseClass;

	explicit CPointsInCoordinateFormatObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item);
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
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_count READ GetCount WRITE SetCount NOTIFY countChanged)
	Q_PROPERTY(QVariant m_offset READ GetOffset WRITE SetOffset NOTIFY offsetChanged)
	Q_PROPERTY(QVariant m_filterModel READ GetFilterModel WRITE SetFilterModel NOTIFY filterModelChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCollectionViewParamsTestObject(QObject* parent = nullptr);

	QVariant GetCount();
	QString Get__Typename() {return QStringLiteral("CollectionViewParamsTest");}
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





class CCollectionViewParamsTestObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCollectionViewParamsTest, sdl::V1_0::complextest::CCollectionViewParamsTestObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::complextest::CCollectionViewParamsTest, sdl::V1_0::complextest::CCollectionViewParamsTestObject> BaseClass;

	explicit CCollectionViewParamsTestObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::complextest::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::complextest::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::complextest::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CExtendedMetaDataUnionTypeObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::complextest::ExtendedMetaDataUnionType, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::complextest::ExtendedMetaDataUnionType, QVariant> BaseClass;

	CExtendedMetaDataUnionTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};



class CExtendedMetaDataUnionSimpleTypeObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::complextest::ExtendedMetaDataUnionSimpleType, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::complextest::ExtendedMetaDataUnionSimpleType, QVariant> BaseClass;

	CExtendedMetaDataUnionSimpleTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};



class CCDMResultUnionTypeObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::complextest::CDMResultUnionType, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::complextest::CDMResultUnionType, QVariant> BaseClass;

	CCDMResultUnionTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterComplexUnion2QmlTypes()
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

} // namespace sdl::V1_0::complextest
