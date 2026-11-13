#pragma once

#include "SubstrateSpecifications.h"

#ifdef QT_QML_LIB
#include <QtQml/QQmlEngine>
#endif // QT_QML_LIB


// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtsdl/TListModelBase.h>

// custom types includes
#if __has_include(<imtbasesdl/SDL/1.0/CPP/ImtBaseTypesObjects.h>)
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypesObjects.h>
#else
#error The objects header <imtbasesdl/SDL/1.0/CPP/ImtBaseTypesObjects.h> is not found. The schema, that defines the imported types, is probably generated without objects (NO_OBJECTS). Generate the objects for it or disable them for this schema.
#endif


namespace sdl::V1_0::substrate
{


class CSubstrateSpecificationDocumentObject;
class CSubstrateSpecificationDocumentObjectList;




class CInputIdObjectList;

class CInputIdObject: public ::imtbase::CItemModelBase, public CInputId
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CInputIdObject(QObject* parent = nullptr);

	QVariant GetId();
	QString Get__Typename() {return QStringLiteral("InputId");}
	void SetId(const QVariant& v);
	Q_INVOKABLE bool hasId();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void idChanged();

protected:
};





class CInputIdObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CInputId, sdl::V1_0::substrate::CInputIdObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CInputId, sdl::V1_0::substrate::CInputIdObject> BaseClass;

	explicit CInputIdObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CInputIdObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CInputIdObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CInputIdObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CInputIdObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CInputIdObject* item);
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

protected:
};





class CCollectionViewParamsTestObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CCollectionViewParamsTest, sdl::V1_0::substrate::CCollectionViewParamsTestObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CCollectionViewParamsTest, sdl::V1_0::substrate::CCollectionViewParamsTestObject> BaseClass;

	explicit CCollectionViewParamsTestObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CCollectionViewParamsTestObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CCollectionViewParamsTestObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSubstrateSpecificationDocumentTypeIdsObjectList;

class CSubstrateSpecificationDocumentTypeIdsObject: public ::imtbase::CItemModelBase, public CSubstrateSpecificationDocumentTypeIds
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_paperID READ GetPaperID WRITE SetPaperID NOTIFY paperIDChanged)
	Q_PROPERTY(QVariant m_cardboardID READ GetCardboardID WRITE SetCardboardID NOTIFY cardboardIDChanged)
	Q_PROPERTY(QVariant m_filmID READ GetFilmID WRITE SetFilmID NOTIFY filmIDChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationDocumentTypeIdsObject(QObject* parent = nullptr);

	QVariant GetPaperID();
	QString Get__Typename() {return QStringLiteral("SubstrateSpecificationDocumentTypeIds");}
	void SetPaperID(const QVariant& v);
	Q_INVOKABLE bool hasPaperID();
	QVariant GetCardboardID();
	void SetCardboardID(const QVariant& v);
	Q_INVOKABLE bool hasCardboardID();
	QVariant GetFilmID();
	void SetFilmID(const QVariant& v);
	Q_INVOKABLE bool hasFilmID();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void paperIDChanged();
	void cardboardIDChanged();
	void filmIDChanged();

protected:
};





class CSubstrateSpecificationDocumentTypeIdsObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIds, sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIds, sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject> BaseClass;

	explicit CSubstrateSpecificationDocumentTypeIdsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIdsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSubstrateSpecificationListInputObjectList;

class CSubstrateSpecificationListInputObject: public ::imtbase::CItemModelBase, public CSubstrateSpecificationListInput
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_viewParams READ GetViewParams WRITE SetViewParams NOTIFY viewParamsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationListInputObject(QObject* parent = nullptr);

	QVariant GetViewParams();
	QString Get__Typename() {return QStringLiteral("SubstrateSpecificationListInput");}
	void SetViewParams(const QVariant& v);
	Q_INVOKABLE bool hasViewParams();
	Q_INVOKABLE void emplaceViewParams();
	void ResetViewParams();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void viewParamsChanged();

protected:
	QVariant m_viewParamsQObjectPtr;
};





class CSubstrateSpecificationListInputObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationListInput, sdl::V1_0::substrate::CSubstrateSpecificationListInputObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationListInput, sdl::V1_0::substrate::CSubstrateSpecificationListInputObject> BaseClass;

	explicit CSubstrateSpecificationListInputObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CSubstrateSpecificationListInputObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CSubstrateSpecificationListInputObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationListInputObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationListInputObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSubstrateSpecificationListItemObjectList;

class CSubstrateSpecificationListItemObject: public ::imtbase::CItemModelBase, public CSubstrateSpecificationListItem
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)
	Q_PROPERTY(QVariant m_typeId READ GetTypeId WRITE SetTypeId NOTIFY typeIdChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_materialId READ GetMaterialId WRITE SetMaterialId NOTIFY materialIdChanged)
	Q_PROPERTY(QVariant m_materialName READ GetMaterialName WRITE SetMaterialName NOTIFY materialNameChanged)
	Q_PROPERTY(QVariant m_added READ GetAdded WRITE SetAdded NOTIFY addedChanged)
	Q_PROPERTY(QVariant m_timeStamp READ GetTimeStamp WRITE SetTimeStamp NOTIFY timeStampChanged)
	Q_PROPERTY(QVariant m_colorPreview READ GetColorPreview WRITE SetColorPreview NOTIFY colorPreviewChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationListItemObject(QObject* parent = nullptr);

	QVariant GetId();
	QString Get__Typename() {return QStringLiteral("SubstrateSpecificationListItem");}
	void SetId(const QVariant& v);
	Q_INVOKABLE bool hasId();
	QVariant GetTypeId();
	void SetTypeId(const QVariant& v);
	Q_INVOKABLE bool hasTypeId();
	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetMaterialId();
	void SetMaterialId(const QVariant& v);
	Q_INVOKABLE bool hasMaterialId();
	QVariant GetMaterialName();
	void SetMaterialName(const QVariant& v);
	Q_INVOKABLE bool hasMaterialName();
	QVariant GetAdded();
	void SetAdded(const QVariant& v);
	Q_INVOKABLE bool hasAdded();
	QVariant GetTimeStamp();
	void SetTimeStamp(const QVariant& v);
	Q_INVOKABLE bool hasTimeStamp();
	QVariant GetColorPreview();
	void SetColorPreview(const QVariant& v);
	Q_INVOKABLE bool hasColorPreview();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void idChanged();
	void typeIdChanged();
	void nameChanged();
	void materialIdChanged();
	void materialNameChanged();
	void addedChanged();
	void timeStampChanged();
	void colorPreviewChanged();

protected:
};





class CSubstrateSpecificationListItemObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationListItem, sdl::V1_0::substrate::CSubstrateSpecificationListItemObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationListItem, sdl::V1_0::substrate::CSubstrateSpecificationListItemObject> BaseClass;

	explicit CSubstrateSpecificationListItemObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CSubstrateSpecificationListItemObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CSubstrateSpecificationListItemObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationListItemObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationListItemObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSubstrateSpecificationListResponseObjectList;

class CSubstrateSpecificationListResponseObject: public ::imtbase::CItemModelBase, public CSubstrateSpecificationListResponse
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_items READ GetItems WRITE SetItems NOTIFY itemsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationListResponseObject(QObject* parent = nullptr);

	QVariant GetItems();
	QString Get__Typename() {return QStringLiteral("SubstrateSpecificationListResponse");}
	void SetItems(const QVariant& v);
	Q_INVOKABLE bool hasItems();
	Q_INVOKABLE void emplaceItems();
	void ResetItems();
	Q_INVOKABLE QVariant createItemsArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void itemsChanged();

protected:
	QVariant m_itemsQObjectPtr;
};





class CSubstrateSpecificationListResponseObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationListResponse, sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationListResponse, sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject> BaseClass;

	explicit CSubstrateSpecificationListResponseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationListResponseObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationListResponseObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CCardboardSubstrateSpecificationObjectList;

class CCardboardSubstrateSpecificationObject: public ::imtbase::CItemModelBase, public CCardboardSubstrateSpecification
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_color READ GetColor WRITE SetColor NOTIFY colorChanged)
	Q_PROPERTY(QVariant m_grammage READ GetGrammage WRITE SetGrammage NOTIFY grammageChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCardboardSubstrateSpecificationObject(QObject* parent = nullptr);

	QVariant GetColor();
	QString Get__Typename() {return QStringLiteral("CardboardSubstrateSpecification");}
	void SetColor(const QVariant& v);
	Q_INVOKABLE bool hasColor();
	QVariant GetGrammage();
	void SetGrammage(const QVariant& v);
	Q_INVOKABLE bool hasGrammage();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void colorChanged();
	void grammageChanged();

protected:
};





class CCardboardSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CCardboardSubstrateSpecification, sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CCardboardSubstrateSpecification, sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject> BaseClass;

	explicit CCardboardSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CCardboardSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CCardboardSubstrateSpecificationObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CPaperSubstrateSpecificationObjectList;

class CPaperSubstrateSpecificationObject: public ::imtbase::CItemModelBase, public CPaperSubstrateSpecification
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_grammage READ GetGrammage WRITE SetGrammage NOTIFY grammageChanged)
	Q_PROPERTY(QVariant m_color READ GetColor WRITE SetColor NOTIFY colorChanged)
	Q_PROPERTY(QVariant m_coatType READ GetCoatType WRITE SetCoatType NOTIFY coatTypeChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPaperSubstrateSpecificationObject(QObject* parent = nullptr);

	QVariant GetGrammage();
	QString Get__Typename() {return QStringLiteral("PaperSubstrateSpecification");}
	void SetGrammage(const QVariant& v);
	Q_INVOKABLE bool hasGrammage();
	QVariant GetColor();
	void SetColor(const QVariant& v);
	Q_INVOKABLE bool hasColor();
	QVariant GetCoatType();
	void SetCoatType(const QVariant& v);
	Q_INVOKABLE bool hasCoatType();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void grammageChanged();
	void colorChanged();
	void coatTypeChanged();

protected:
};





class CPaperSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CPaperSubstrateSpecification, sdl::V1_0::substrate::CPaperSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CPaperSubstrateSpecification, sdl::V1_0::substrate::CPaperSubstrateSpecificationObject> BaseClass;

	explicit CPaperSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CPaperSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CPaperSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CPaperSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CPaperSubstrateSpecificationObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CFilmSubstrateSpecificationObjectList;

class CFilmSubstrateSpecificationObject: public ::imtbase::CItemModelBase, public CFilmSubstrateSpecification
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_transparency READ GetTransparency WRITE SetTransparency NOTIFY transparencyChanged)
	Q_PROPERTY(QVariant m_color READ GetColor WRITE SetColor NOTIFY colorChanged)
	Q_PROPERTY(QVariant m_material READ GetMaterial WRITE SetMaterial NOTIFY materialChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFilmSubstrateSpecificationObject(QObject* parent = nullptr);

	QVariant GetTransparency();
	QString Get__Typename() {return QStringLiteral("FilmSubstrateSpecification");}
	void SetTransparency(const QVariant& v);
	Q_INVOKABLE bool hasTransparency();
	QVariant GetColor();
	void SetColor(const QVariant& v);
	Q_INVOKABLE bool hasColor();
	QVariant GetMaterial();
	void SetMaterial(const QVariant& v);
	Q_INVOKABLE bool hasMaterial();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void transparencyChanged();
	void colorChanged();
	void materialChanged();

protected:
};





class CFilmSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CFilmSubstrateSpecification, sdl::V1_0::substrate::CFilmSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CFilmSubstrateSpecification, sdl::V1_0::substrate::CFilmSubstrateSpecificationObject> BaseClass;

	explicit CFilmSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CFilmSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CFilmSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CFilmSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CFilmSubstrateSpecificationObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSubstrateSpecificationInputObjectList;

class CSubstrateSpecificationInputObject: public ::imtbase::CItemModelBase, public CSubstrateSpecificationInput
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QVariant m_item READ GetItem WRITE SetItem NOTIFY itemChanged)
	Q_PROPERTY(QVariant m_typeId READ GetTypeId WRITE SetTypeId NOTIFY typeIdChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationInputObject(QObject* parent = nullptr);

	QVariant GetId();
	QString Get__Typename() {return QStringLiteral("SubstrateSpecificationInput");}
	void SetId(const QVariant& v);
	Q_INVOKABLE bool hasId();
	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetDescription();
	void SetDescription(const QVariant& v);
	Q_INVOKABLE bool hasDescription();
	QVariant GetItem();
	void SetItem(const QVariant& v);
	Q_INVOKABLE bool hasItem();
	Q_INVOKABLE void emplaceItem();
	void ResetItem();
	QVariant GetTypeId();
	void SetTypeId(const QVariant& v);
	Q_INVOKABLE bool hasTypeId();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void idChanged();
	void nameChanged();
	void descriptionChanged();
	void itemChanged();
	void typeIdChanged();

protected:
	QVariant m_itemQObjectPtr;
};





class CSubstrateSpecificationInputObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationInput, sdl::V1_0::substrate::CSubstrateSpecificationInputObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationInput, sdl::V1_0::substrate::CSubstrateSpecificationInputObject> BaseClass;

	explicit CSubstrateSpecificationInputObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CSubstrateSpecificationInputObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CSubstrateSpecificationInputObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationInputObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationInputObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSubstrateSpecificationDocumentMetaInfoObjectList;

class CSubstrateSpecificationDocumentMetaInfoObject: public ::imtbase::CItemModelBase, public CSubstrateSpecificationDocumentMetaInfo
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_materialId READ GetMaterialId WRITE SetMaterialId NOTIFY materialIdChanged)
	Q_PROPERTY(QVariant m_lab READ GetLab WRITE SetLab NOTIFY labChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationDocumentMetaInfoObject(QObject* parent = nullptr);

	QVariant GetMaterialId();
	QString Get__Typename() {return QStringLiteral("SubstrateSpecificationDocumentMetaInfo");}
	void SetMaterialId(const QVariant& v);
	Q_INVOKABLE bool hasMaterialId();
	QVariant GetLab();
	void SetLab(const QVariant& v);
	Q_INVOKABLE bool hasLab();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void materialIdChanged();
	void labChanged();

protected:
};





class CSubstrateSpecificationDocumentMetaInfoObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfo, sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfo, sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject> BaseClass;

	explicit CSubstrateSpecificationDocumentMetaInfoObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSubstrateSpecificationDocumentObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::substrate::SubstrateSpecificationDocument, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::substrate::SubstrateSpecificationDocument, QVariant> BaseClass;

	CSubstrateSpecificationDocumentObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::substrate::CSubstrateSpecificationDocumentObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::substrate::CSubstrateSpecificationDocumentObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterSubstrateSpecificationsQmlTypes()
{
	qmlRegisterType<CInputIdObject>("substrateSubstrateSpecificationsSdl", 1, 0, "InputId");
	qmlRegisterType<CCollectionViewParamsTestObject>("substrateSubstrateSpecificationsSdl", 1, 0, "CollectionViewParamsTest");
	qmlRegisterType<CSubstrateSpecificationDocumentTypeIdsObject>("substrateSubstrateSpecificationsSdl", 1, 0, "SubstrateSpecificationDocumentTypeIds");
	qmlRegisterType<CSubstrateSpecificationListInputObject>("substrateSubstrateSpecificationsSdl", 1, 0, "SubstrateSpecificationListInput");
	qmlRegisterType<CSubstrateSpecificationListItemObject>("substrateSubstrateSpecificationsSdl", 1, 0, "SubstrateSpecificationListItem");
	qmlRegisterType<CSubstrateSpecificationListResponseObject>("substrateSubstrateSpecificationsSdl", 1, 0, "SubstrateSpecificationListResponse");
	qmlRegisterType<CCardboardSubstrateSpecificationObject>("substrateSubstrateSpecificationsSdl", 1, 0, "CardboardSubstrateSpecification");
	qmlRegisterType<CPaperSubstrateSpecificationObject>("substrateSubstrateSpecificationsSdl", 1, 0, "PaperSubstrateSpecification");
	qmlRegisterType<CFilmSubstrateSpecificationObject>("substrateSubstrateSpecificationsSdl", 1, 0, "FilmSubstrateSpecification");
	qmlRegisterType<CSubstrateSpecificationInputObject>("substrateSubstrateSpecificationsSdl", 1, 0, "SubstrateSpecificationInput");
	qmlRegisterType<CSubstrateSpecificationDocumentMetaInfoObject>("substrateSubstrateSpecificationsSdl", 1, 0, "SubstrateSpecificationDocumentMetaInfo");
}
#endif

} // namespace sdl::V1_0::substrate
