#pragma once

#include "SubstrateSpecifications_fwd.h"

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
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace sdl::V1_0::V1_0
{


// forward declaration for union types
class SubstrateSpecificationDocument;
class CSubstrateSpecificationDocumentObject;
class CSubstrateSpecificationDocumentObjectList;

class CInputId
{
public:
	struct InputIdFields
	{
		static const inline QString Id = "id";
	};

	istd::TSharedNullable<QByteArray> id;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CInputId& other) const;
	[[nodiscard]] bool operator!=(const CInputId& other) const {return !(operator==(other));}
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
	};

	istd::TSharedNullable<int> count;
	istd::TSharedNullable<int> offset;

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

class CSubstrateSpecificationDocumentTypeIds
{
public:
	struct SubstrateSpecificationDocumentTypeIdsFields
	{
		static const inline QString PaperID = "PaperID";
		static const inline QString CardboardID = "CardboardID";
		static const inline QString FilmID = "FilmID";
	};

	istd::TSharedNullable<QByteArray> PaperID;
	istd::TSharedNullable<QByteArray> CardboardID;
	istd::TSharedNullable<QByteArray> FilmID;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSubstrateSpecificationDocumentTypeIds& other) const;
	[[nodiscard]] bool operator!=(const CSubstrateSpecificationDocumentTypeIds& other) const {return !(operator==(other));}
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

class CSubstrateSpecificationListInput
{
public:
	struct SubstrateSpecificationListInputFields
	{
		static const inline QString ViewParams = "viewParams";
	};

	istd::TSharedNullable<CCollectionViewParamsTest> viewParams;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSubstrateSpecificationListInput& other) const;
	[[nodiscard]] bool operator!=(const CSubstrateSpecificationListInput& other) const {return !(operator==(other));}
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

class CSubstrateSpecificationListItem
{
public:
	struct SubstrateSpecificationListItemFields
	{
		static const inline QString Id = "id";
		static const inline QString TypeId = "typeId";
		static const inline QString Name = "name";
		static const inline QString MaterialId = "materialId";
		static const inline QString MaterialName = "materialName";
		static const inline QString Added = "added";
		static const inline QString TimeStamp = "timeStamp";
		static const inline QString ColorPreview = "colorPreview";
	};

	istd::TSharedNullable<QByteArray> id;
	istd::TSharedNullable<QByteArray> typeId;
	istd::TSharedNullable<QString> name;
	istd::TSharedNullable<QByteArray> materialId;
	istd::TSharedNullable<QString> materialName;
	istd::TSharedNullable<QString> added;
	istd::TSharedNullable<QString> timeStamp;
	istd::TSharedNullable<QString> colorPreview;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSubstrateSpecificationListItem& other) const;
	[[nodiscard]] bool operator!=(const CSubstrateSpecificationListItem& other) const {return !(operator==(other));}
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

class CSubstrateSpecificationListResponse
{
public:
	struct SubstrateSpecificationListResponseFields
	{
		static const inline QString Items = "items";
	};

	istd::TSharedNullable<imtsdl::TElementList<CSubstrateSpecificationListItem>> items;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSubstrateSpecificationListResponse& other) const;
	[[nodiscard]] bool operator!=(const CSubstrateSpecificationListResponse& other) const {return !(operator==(other));}
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

class CCardboardSubstrateSpecification
{
public:
	struct CardboardSubstrateSpecificationFields
	{
		static const inline QString Color = "color";
		static const inline QString Grammage = "grammage";
	};

	istd::TSharedNullable<QString> color;
	istd::TSharedNullable<double> grammage;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CCardboardSubstrateSpecification& other) const;
	[[nodiscard]] bool operator!=(const CCardboardSubstrateSpecification& other) const {return !(operator==(other));}
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

class CPaperSubstrateSpecification
{
public:
	struct PaperSubstrateSpecificationFields
	{
		static const inline QString Grammage = "grammage";
		static const inline QString Color = "color";
		static const inline QString CoatType = "coatType";
	};

	istd::TSharedNullable<double> grammage;
	istd::TSharedNullable<QString> color;
	istd::TSharedNullable<QString> coatType;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CPaperSubstrateSpecification& other) const;
	[[nodiscard]] bool operator!=(const CPaperSubstrateSpecification& other) const {return !(operator==(other));}
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

class CFilmSubstrateSpecification
{
public:
	struct FilmSubstrateSpecificationFields
	{
		static const inline QString Transparency = "transparency";
		static const inline QString Color = "color";
		static const inline QString Material = "material";
	};

	istd::TSharedNullable<double> transparency;
	istd::TSharedNullable<QString> color;
	istd::TSharedNullable<QString> material;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CFilmSubstrateSpecification& other) const;
	[[nodiscard]] bool operator!=(const CFilmSubstrateSpecification& other) const {return !(operator==(other));}
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

class CSubstrateSpecificationInput
{
public:
	struct SubstrateSpecificationInputFields
	{
		static const inline QString Id = "id";
		static const inline QString Name = "name";
		static const inline QString Description = "description";
		static const inline QString Item = "item";
		static const inline QString TypeId = "typeId";
	};

	istd::TSharedNullable<QByteArray> id;
	istd::TSharedNullable<QString> name;
	istd::TSharedNullable<QString> description;
	istd::TSharedNullable<SubstrateSpecificationDocument> item;
	istd::TSharedNullable<QByteArray> typeId;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSubstrateSpecificationInput& other) const;
	[[nodiscard]] bool operator!=(const CSubstrateSpecificationInput& other) const {return !(operator==(other));}
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

class CSubstrateSpecificationDocumentMetaInfo
{
public:
	struct SubstrateSpecificationDocumentMetaInfoFields
	{
		static const inline QString MaterialId = "materialId";
		static const inline QString Lab = "lab";
	};

	istd::TSharedNullable<QByteArray> materialId;
	istd::TSharedNullable<QString> lab;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSubstrateSpecificationDocumentMetaInfo& other) const;
	[[nodiscard]] bool operator!=(const CSubstrateSpecificationDocumentMetaInfo& other) const {return !(operator==(other));}
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




class CInputIdObjectList;

class CInputIdObject: public ::imtbase::CItemModelBase, public CInputId
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CInputIdObject(QObject* parent = nullptr);

	QVariant GetId();
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





class CInputIdObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CInputId, sdl::V1_0::V1_0::CInputIdObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CInputId, sdl::V1_0::V1_0::CInputIdObject> BaseClass;

	explicit CInputIdObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CInputIdObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CInputIdObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CInputIdObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CInputIdObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CInputIdObject* item);
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

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCollectionViewParamsTestObject(QObject* parent = nullptr);

	QVariant GetCount();
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





class CCollectionViewParamsTestObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CCollectionViewParamsTest, sdl::V1_0::V1_0::CCollectionViewParamsTestObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CCollectionViewParamsTest, sdl::V1_0::V1_0::CCollectionViewParamsTestObject> BaseClass;

	explicit CCollectionViewParamsTestObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CCollectionViewParamsTestObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CCollectionViewParamsTestObject* item);
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
	Q_PROPERTY(QVariant m_paperID READ GetPaperID WRITE SetPaperID NOTIFY paperIDChanged)
	Q_PROPERTY(QVariant m_cardboardID READ GetCardboardID WRITE SetCardboardID NOTIFY cardboardIDChanged)
	Q_PROPERTY(QVariant m_filmID READ GetFilmID WRITE SetFilmID NOTIFY filmIDChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationDocumentTypeIdsObject(QObject* parent = nullptr);

	QVariant GetPaperID();
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





class CSubstrateSpecificationDocumentTypeIdsObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIds, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIds, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject> BaseClass;

	explicit CSubstrateSpecificationDocumentTypeIdsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIdsObject* item);
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
	Q_PROPERTY(QVariant m_viewParams READ GetViewParams WRITE SetViewParams NOTIFY viewParamsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationListInputObject(QObject* parent = nullptr);

	QVariant GetViewParams();
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





class CSubstrateSpecificationListInputObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationListInput, sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationListInput, sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject> BaseClass;

	explicit CSubstrateSpecificationListInputObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationListInputObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationListInputObject* item);
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





class CSubstrateSpecificationListItemObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationListItem, sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationListItem, sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject> BaseClass;

	explicit CSubstrateSpecificationListItemObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationListItemObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationListItemObject* item);
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
	Q_PROPERTY(QVariant m_items READ GetItems WRITE SetItems NOTIFY itemsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationListResponseObject(QObject* parent = nullptr);

	QVariant GetItems();
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





class CSubstrateSpecificationListResponseObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationListResponse, sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationListResponse, sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject> BaseClass;

	explicit CSubstrateSpecificationListResponseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationListResponseObject* item);
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
	Q_PROPERTY(QVariant m_color READ GetColor WRITE SetColor NOTIFY colorChanged)
	Q_PROPERTY(QVariant m_grammage READ GetGrammage WRITE SetGrammage NOTIFY grammageChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCardboardSubstrateSpecificationObject(QObject* parent = nullptr);

	QVariant GetColor();
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





class CCardboardSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CCardboardSubstrateSpecification, sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CCardboardSubstrateSpecification, sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject> BaseClass;

	explicit CCardboardSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CCardboardSubstrateSpecificationObject* item);
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
	Q_PROPERTY(QVariant m_grammage READ GetGrammage WRITE SetGrammage NOTIFY grammageChanged)
	Q_PROPERTY(QVariant m_color READ GetColor WRITE SetColor NOTIFY colorChanged)
	Q_PROPERTY(QVariant m_coatType READ GetCoatType WRITE SetCoatType NOTIFY coatTypeChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPaperSubstrateSpecificationObject(QObject* parent = nullptr);

	QVariant GetGrammage();
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





class CPaperSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CPaperSubstrateSpecification, sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CPaperSubstrateSpecification, sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject> BaseClass;

	explicit CPaperSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CPaperSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CPaperSubstrateSpecificationObject* item);
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
	Q_PROPERTY(QVariant m_transparency READ GetTransparency WRITE SetTransparency NOTIFY transparencyChanged)
	Q_PROPERTY(QVariant m_color READ GetColor WRITE SetColor NOTIFY colorChanged)
	Q_PROPERTY(QVariant m_material READ GetMaterial WRITE SetMaterial NOTIFY materialChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFilmSubstrateSpecificationObject(QObject* parent = nullptr);

	QVariant GetTransparency();
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





class CFilmSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CFilmSubstrateSpecification, sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CFilmSubstrateSpecification, sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject> BaseClass;

	explicit CFilmSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CFilmSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CFilmSubstrateSpecificationObject* item);
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
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QVariant m_item READ GetItem WRITE SetItem NOTIFY itemChanged)
	Q_PROPERTY(QVariant m_typeId READ GetTypeId WRITE SetTypeId NOTIFY typeIdChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationInputObject(QObject* parent = nullptr);

	QVariant GetId();
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





class CSubstrateSpecificationInputObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationInput, sdl::V1_0::V1_0::CSubstrateSpecificationInputObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationInput, sdl::V1_0::V1_0::CSubstrateSpecificationInputObject> BaseClass;

	explicit CSubstrateSpecificationInputObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CSubstrateSpecificationInputObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CSubstrateSpecificationInputObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationInputObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationInputObject* item);
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
	Q_PROPERTY(QVariant m_materialId READ GetMaterialId WRITE SetMaterialId NOTIFY materialIdChanged)
	Q_PROPERTY(QVariant m_lab READ GetLab WRITE SetLab NOTIFY labChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSubstrateSpecificationDocumentMetaInfoObject(QObject* parent = nullptr);

	QVariant GetMaterialId();
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





class CSubstrateSpecificationDocumentMetaInfoObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfo, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfo, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject> BaseClass;

	explicit CSubstrateSpecificationDocumentMetaInfoObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};


class SubstrateSpecificationDocument: public std::variant<CCardboardSubstrateSpecification, CPaperSubstrateSpecification, CFilmSubstrateSpecification> {

public:

	typedef std::variant<CCardboardSubstrateSpecification, CPaperSubstrateSpecification, CFilmSubstrateSpecification> BaseClass;

	SubstrateSpecificationDocument(){};
	SubstrateSpecificationDocument(const CCardboardSubstrateSpecification& ref)
		: BaseClass(ref){};

	SubstrateSpecificationDocument(const CPaperSubstrateSpecification& ref)
		: BaseClass(ref){};

	SubstrateSpecificationDocument(const CFilmSubstrateSpecification& ref)
		: BaseClass(ref){};

};




class CSubstrateSpecificationDocumentObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::V1_0::SubstrateSpecificationDocument, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::V1_0::SubstrateSpecificationDocument, QVariant> BaseClass;

	CSubstrateSpecificationDocumentObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::V1_0::CSubstrateSpecificationDocumentObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};


struct GetSubstrateSpecificationListRequestInfo
{
	bool isItemsRequested = true;
		struct itemsRequestInfo
		{
			bool isIdRequested = true;
			bool isTypeIdRequested = true;
			bool isNameRequested = true;
			bool isMaterialIdRequested = true;
			bool isMaterialNameRequested = true;
			bool isAddedRequested = true;
			bool isTimeStampRequested = true;
			bool isColorPreviewRequested = true;
		} items;
};


struct GetSubstrateSpecificationListRequestArguments
{
	istd::TSharedNullable<CSubstrateSpecificationListInput> input;
};


class CGetSubstrateSpecificationListGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetSubstrateSpecificationListRequestArguments& requestArguments, const GetSubstrateSpecificationListRequestInfo& requestInfo = GetSubstrateSpecificationListRequestInfo());

	CGetSubstrateSpecificationListGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetSubstrateSpecificationListRequestArguments& GetRequestedArguments() const;
	GetSubstrateSpecificationListRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetSubstrateSpecificationListRequestArguments m_requestedArguments;
	GetSubstrateSpecificationListRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};




struct GetSubstrateSpecificationRequestInfo
{
};


struct GetSubstrateSpecificationRequestArguments
{
	istd::TSharedNullable<CInputId> input;
};


class CGetSubstrateSpecificationGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetSubstrateSpecificationRequestArguments& requestArguments, const GetSubstrateSpecificationRequestInfo& requestInfo = GetSubstrateSpecificationRequestInfo());

	CGetSubstrateSpecificationGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetSubstrateSpecificationRequestArguments& GetRequestedArguments() const;
	GetSubstrateSpecificationRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetSubstrateSpecificationRequestArguments m_requestedArguments;
	GetSubstrateSpecificationRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};




struct GetOptionsListRequestInfo
{
	bool isTotalCountRequested = true;
	bool isOffsetRequested = true;
	bool isOptionsRequested = true;
		struct optionsRequestInfo
		{
			bool isIdRequested = true;
			bool isNameRequested = true;
			bool isDescriptionRequested = true;
			bool isEnabledRequested = true;
		} options;
};


struct GetOptionsListRequestArguments
{
	istd::TSharedNullable<CCollectionViewParamsTest> input;
};


class CGetOptionsListGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetOptionsListRequestArguments& requestArguments, const GetOptionsListRequestInfo& requestInfo = GetOptionsListRequestInfo());

	CGetOptionsListGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetOptionsListRequestArguments& GetRequestedArguments() const;
	GetOptionsListRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetOptionsListRequestArguments m_requestedArguments;
	GetOptionsListRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};




struct InsertSubstrateSpecificationRequestInfo
{
	bool isIdRequested = true;
};


struct InsertSubstrateSpecificationRequestArguments
{
	istd::TSharedNullable<CSubstrateSpecificationInput> input;
};


class CInsertSubstrateSpecificationGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const InsertSubstrateSpecificationRequestArguments& requestArguments, const InsertSubstrateSpecificationRequestInfo& requestInfo = InsertSubstrateSpecificationRequestInfo());

	CInsertSubstrateSpecificationGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const InsertSubstrateSpecificationRequestArguments& GetRequestedArguments() const;
	InsertSubstrateSpecificationRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	InsertSubstrateSpecificationRequestArguments m_requestedArguments;
	InsertSubstrateSpecificationRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};




struct UpdateSubstrateSpecificationRequestInfo
{
	bool isIdRequested = true;
};


struct UpdateSubstrateSpecificationRequestArguments
{
	istd::TSharedNullable<CSubstrateSpecificationInput> input;
};


class CUpdateSubstrateSpecificationGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const UpdateSubstrateSpecificationRequestArguments& requestArguments, const UpdateSubstrateSpecificationRequestInfo& requestInfo = UpdateSubstrateSpecificationRequestInfo());

	CUpdateSubstrateSpecificationGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const UpdateSubstrateSpecificationRequestArguments& GetRequestedArguments() const;
	UpdateSubstrateSpecificationRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	UpdateSubstrateSpecificationRequestArguments m_requestedArguments;
	UpdateSubstrateSpecificationRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};





#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
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


Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CInputId);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CCollectionViewParamsTest);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentTypeIds);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CSubstrateSpecificationListInput);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CSubstrateSpecificationListItem);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CSubstrateSpecificationListResponse);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CCardboardSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CPaperSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CFilmSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CSubstrateSpecificationInput);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CSubstrateSpecificationDocumentMetaInfo);
