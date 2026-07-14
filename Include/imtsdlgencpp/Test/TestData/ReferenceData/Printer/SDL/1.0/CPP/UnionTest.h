#pragma once

#include "UnionTest_fwd.h"

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
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtsdl/TListModelBase.h>
#include <imtsdl/TElementList.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::V1_0::modsdl
{


inline namespace UnionTestSdlEnums
{



Q_NAMESPACE

enum class LinkStatus {
	OPEN,
	CLOSE,
};

Q_ENUM_NS(LinkStatus)


class EnumLinkStatus: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString OPEN READ GetOPEN NOTIFY OPENChanged)
	Q_PROPERTY(QString CLOSE READ GetCLOSE NOTIFY CLOSEChanged)

protected:
	static QString GetOPEN() { return "OPEN"; }
	static QString GetCLOSE() { return "CLOSE"; }

signals:
	void OPENChanged();
	void CLOSEChanged();
};



} // inline namespace UnionTestSdlEnums

// forward declaration for union types
class PrinterSpecification;
class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;

class CCoords
{
public:
	struct CoordsFields
	{
		static const inline QString X = "X";
		static const inline QString Y = "Y";
	};

	istd::TNullableValue<double> X;
	istd::TNullableValue<double> Y;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CCoords& other) const;
	[[nodiscard]] bool operator!=(const CCoords& other) const {return !(operator==(other));}
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

class CPrinterSpecificationBase
{
public:
	struct PrinterSpecificationBaseFields
	{
		static const inline QString Name = "name";
	};

	istd::TNullableValue<QString> name;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CPrinterSpecificationBase& other) const;
	[[nodiscard]] bool operator!=(const CPrinterSpecificationBase& other) const {return !(operator==(other));}
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

class CLink
{
public:
	struct LinkFields
	{
		static const inline QString Link = "link";
		static const inline QString Status = "status";
		static const inline QString StatusList = "statusList";
	};

	istd::TNullableValue<QByteArray> link;
	istd::TNullableValue<LinkStatus> status;
	istd::TNullableValue<imtsdl::TElementList<LinkStatus>> statusList;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CLink& other) const;
	[[nodiscard]] bool operator!=(const CLink& other) const {return !(operator==(other));}
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

class CPrinterBase
{
public:
	struct PrinterBaseFields
	{
		static const inline QString Name = "name";
		static const inline QString LinkList = "linkList";
		static const inline QString Specification = "specification";
		static const inline QString SpecificationList = "specificationList";
	};

	istd::TNullableValue<QString> name;
	istd::TNullableValue<imtsdl::TElementList<CLink>> linkList;
	istd::TNullableValue<PrinterSpecification> specification;
	istd::TNullableValue<imtsdl::TElementList<PrinterSpecification>> specificationList;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CPrinterBase& other) const;
	[[nodiscard]] bool operator!=(const CPrinterBase& other) const {return !(operator==(other));}
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




class CCoordsObjectList;

class CCoordsObject: public ::imtbase::CItemModelBase, public CCoords
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(QVariant m_y READ GetY WRITE SetY NOTIFY yChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CCoordsObject(QObject* parent = nullptr);

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
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterSpecificationBaseObject(QObject* parent = nullptr);

	QVariant GetName();
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
	Q_PROPERTY(QVariant m_link READ GetLink WRITE SetLink NOTIFY linkChanged)
	Q_PROPERTY(QVariant m_status READ GetStatus WRITE SetStatus NOTIFY statusChanged)
	Q_PROPERTY(QVariant m_statusList READ GetStatusList WRITE SetStatusList NOTIFY statusListChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CLinkObject(QObject* parent = nullptr);

	QVariant GetLink();
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
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_linkList READ GetLinkList WRITE SetLinkList NOTIFY linkListChanged)
	Q_PROPERTY(QVariant m_specification READ GetSpecification WRITE SetSpecification NOTIFY specificationChanged)
	Q_PROPERTY(QVariant m_specificationList READ GetSpecificationList WRITE SetSpecificationList NOTIFY specificationListChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterBaseObject(QObject* parent = nullptr);

	QVariant GetName();
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


class PrinterSpecification: public std::variant<CPrinterSpecificationBase, CLink, QString, double> {

public:

	typedef std::variant<CPrinterSpecificationBase, CLink, QString, double> BaseClass;

	PrinterSpecification(){};
	PrinterSpecification(const CPrinterSpecificationBase& ref)
		: BaseClass(ref){};

	PrinterSpecification(const CLink& ref)
		: BaseClass(ref){};

	PrinterSpecification(const QString& ref)
		: BaseClass(ref){};

	PrinterSpecification(const double& ref)
		: BaseClass(ref){};

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


struct GetSpecificationsRequestInfo
{
};


struct GetSpecificationsRequestArguments
{
	istd::TNullableValue<V1_0::modsdl::CLink> input;
};


class CGetSpecificationsGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetSpecificationsRequestArguments& requestArguments, const GetSpecificationsRequestInfo& requestInfo = GetSpecificationsRequestInfo());

	CGetSpecificationsGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetSpecificationsRequestArguments& GetRequestedArguments() const;
	GetSpecificationsRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetSpecificationsRequestArguments m_requestedArguments;
	GetSpecificationsRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
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


Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CCoords);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CPrinterSpecificationBase);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CLink);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CPrinterBase);
