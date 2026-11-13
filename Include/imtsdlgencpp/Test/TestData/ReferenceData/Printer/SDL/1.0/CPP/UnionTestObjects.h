#pragma once

#include "UnionTest.h"

#ifdef QT_QML_LIB
#include <QtQml/QQmlEngine>
#endif // QT_QML_LIB


// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtsdl/TListModelBase.h>


namespace sdl::V1_0::modsdl
{


class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;




class CCoordsObjectList;

class CCoordsObject: public ::imtbase::CItemModelBase, public CCoords
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(QVariant m_y READ GetY WRITE SetY NOTIFY yChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCoordsObject(QObject* parent = nullptr);

	QVariant GetX();
	QString Get__Typename() {return QStringLiteral("Coords");}
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





class CCoordsObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CCoords, sdl::V1_0::modsdl::CCoordsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CCoords, sdl::V1_0::modsdl::CCoordsObject> BaseClass;

	explicit CCoordsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::modsdl::CCoordsObject* item);
	Q_INVOKABLE sdl::V1_0::modsdl::CCoordsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::modsdl::CCoordsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CCoordsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::modsdl::CCoordsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CPrinterSpecificationBaseObjectList;

class CPrinterSpecificationBaseObject: public ::imtbase::CItemModelBase, public CPrinterSpecificationBase
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterSpecificationBaseObject(QObject* parent = nullptr);

	QVariant GetName();
	QString Get__Typename() {return QStringLiteral("PrinterSpecificationBase");}
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void nameChanged();

protected:
};





class CPrinterSpecificationBaseObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CPrinterSpecificationBase, sdl::V1_0::modsdl::CPrinterSpecificationBaseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CPrinterSpecificationBase, sdl::V1_0::modsdl::CPrinterSpecificationBaseObject> BaseClass;

	explicit CPrinterSpecificationBaseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item);
	Q_INVOKABLE sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CPrinterSpecificationBaseObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CLinkObjectList;

class CLinkObject: public ::imtbase::CItemModelBase, public CLink
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_link READ GetLink WRITE SetLink NOTIFY linkChanged)
	Q_PROPERTY(QVariant m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QVariant m_statusList READ GetStatusList WRITE SetStatusList NOTIFY statusListChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CLinkObject(QObject* parent = nullptr);

	QVariant GetLink();
	QString Get__Typename() {return QStringLiteral("Link");}
	void SetLink(const QVariant& v);
	Q_INVOKABLE bool hasLink();
	QVariant GetStatus();
	void SetStatus(const QVariant& v);
	Q_INVOKABLE bool hasStatus();
	QVariant GetStatusList();
	void SetStatusList(const QVariant& v);
	Q_INVOKABLE bool hasStatusList();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void linkChanged();
	void statusChanged();
	void statusListChanged();

protected:
};





class CLinkObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CLink, sdl::V1_0::modsdl::CLinkObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CLink, sdl::V1_0::modsdl::CLinkObject> BaseClass;

	explicit CLinkObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::modsdl::CLinkObject* item);
	Q_INVOKABLE sdl::V1_0::modsdl::CLinkObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::modsdl::CLinkObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CLinkObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::modsdl::CLinkObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CPrinterBaseObjectList;

class CPrinterBaseObject: public ::imtbase::CItemModelBase, public CPrinterBase
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_linkList READ GetLinkList WRITE SetLinkList NOTIFY linkListChanged)
	Q_PROPERTY(QVariant m_specification READ GetSpecification WRITE SetSpecification NOTIFY specificationChanged)
	Q_PROPERTY(QVariant m_specificationList READ GetSpecificationList WRITE SetSpecificationList NOTIFY specificationListChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterBaseObject(QObject* parent = nullptr);

	QVariant GetName();
	QString Get__Typename() {return QStringLiteral("PrinterBase");}
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetLinkList();
	void SetLinkList(const QVariant& v);
	Q_INVOKABLE bool hasLinkList();
	Q_INVOKABLE void emplaceLinkList();
	void ResetLinkList();
	Q_INVOKABLE QVariant createLinkListArrayElement(const QVariant& v);
	QVariant GetSpecification();
	void SetSpecification(const QVariant& v);
	Q_INVOKABLE bool hasSpecification();
	Q_INVOKABLE void emplaceSpecification();
	void ResetSpecification();
	QVariant GetSpecificationList();
	void SetSpecificationList(const QVariant& v);
	Q_INVOKABLE bool hasSpecificationList();
	Q_INVOKABLE void emplaceSpecificationList();
	void ResetSpecificationList();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void nameChanged();
	void linkListChanged();
	void specificationChanged();
	void specificationListChanged();

protected:
	QVariant m_linkListQObjectPtr;
	QVariant m_specificationQObjectPtr;
	QVariant m_specificationListQObjectPtr;
};





class CPrinterBaseObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CPrinterBase, sdl::V1_0::modsdl::CPrinterBaseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CPrinterBase, sdl::V1_0::modsdl::CPrinterBaseObject> BaseClass;

	explicit CPrinterBaseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::modsdl::CPrinterBaseObject* item);
	Q_INVOKABLE sdl::V1_0::modsdl::CPrinterBaseObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::modsdl::CPrinterBaseObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CPrinterBaseObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::modsdl::CPrinterBaseObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CPrinterSpecificationObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::modsdl::PrinterSpecification, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::modsdl::PrinterSpecification, QVariant> BaseClass;

	CPrinterSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::modsdl::CPrinterSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CPrinterSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterUnionTestQmlTypes()
{
	qmlRegisterType<CCoordsObject>("modsdlUnionTestSdl", 1, 0, "Coords");
	qmlRegisterType<CPrinterSpecificationBaseObject>("modsdlUnionTestSdl", 1, 0, "PrinterSpecificationBase");
	qmlRegisterType<CLinkObject>("modsdlUnionTestSdl", 1, 0, "Link");
	qmlRegisterType<CPrinterBaseObject>("modsdlUnionTestSdl", 1, 0, "PrinterBase");
	qmlRegisterSingletonType<EnumLinkStatus>("modsdlUnionTestSdl", 1, 0, "LinkStatus", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumLinkStatus *enumType = new EnumLinkStatus();
		return enumType;
	});
}
#endif

} // namespace sdl::V1_0::modsdl
