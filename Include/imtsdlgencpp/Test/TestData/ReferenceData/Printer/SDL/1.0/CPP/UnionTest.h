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
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::modsdl::UnionTest
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


// forward declaration for union types
class PrinterSpecification;
class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;

class CCoords
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
		struct CoordsFields
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

class CPrinterSpecificationBase
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
		struct PrinterSpecificationBaseFields
		{
			static const inline QString Name = "name";
		};

		istd::TSharedNullable<QString> name;

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

class CLink
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
		struct LinkFields
		{
			static const inline QString Link = "link";
			static const inline QString Status = "status";
			static const inline QString StatusList = "statusList";
		};

		istd::TSharedNullable<QByteArray> link;
		istd::TSharedNullable<LinkStatus> status;
		istd::TSharedNullable<imtsdl::TElementList<LinkStatus>> statusList;

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

class CPrinterBase
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
		struct PrinterBaseFields
		{
			static const inline QString Name = "name";
			static const inline QString LinkList = "linkList";
			static const inline QString Specification = "specification";
			static const inline QString SpecificationList = "specificationList";
		};

		istd::TSharedNullable<QString> name;
		istd::TSharedNullable<imtsdl::TElementList<CLink::V1_0>> linkList;
		istd::TSharedNullable<PrinterSpecification> specification;
		istd::TSharedNullable<imtsdl::TElementList<PrinterSpecification>> specificationList;

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





class CCoordsObjectList: public ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CCoords::V1_0, sdl::modsdl::UnionTest::CCoordsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CCoords::V1_0, sdl::modsdl::UnionTest::CCoordsObject> BaseClass;

	explicit CCoordsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::modsdl::UnionTest::CCoordsObject* item);
	Q_INVOKABLE sdl::modsdl::UnionTest::CCoordsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::modsdl::UnionTest::CCoordsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::UnionTest::CCoordsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::modsdl::UnionTest::CCoordsObject* item);
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





class CPrinterSpecificationBaseObjectList: public ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CPrinterSpecificationBase::V1_0, sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CPrinterSpecificationBase::V1_0, sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject> BaseClass;

	explicit CPrinterSpecificationBaseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject* item);
	Q_INVOKABLE sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::UnionTest::CPrinterSpecificationBaseObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::modsdl::UnionTest::CPrinterSpecificationBaseObject* item);
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





class CLinkObjectList: public ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CLink::V1_0, sdl::modsdl::UnionTest::CLinkObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CLink::V1_0, sdl::modsdl::UnionTest::CLinkObject> BaseClass;

	explicit CLinkObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::modsdl::UnionTest::CLinkObject* item);
	Q_INVOKABLE sdl::modsdl::UnionTest::CLinkObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::modsdl::UnionTest::CLinkObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::UnionTest::CLinkObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::modsdl::UnionTest::CLinkObject* item);
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





class CPrinterBaseObjectList: public ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CPrinterBase::V1_0, sdl::modsdl::UnionTest::CPrinterBaseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::modsdl::UnionTest::CPrinterBase::V1_0, sdl::modsdl::UnionTest::CPrinterBaseObject> BaseClass;

	explicit CPrinterBaseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::modsdl::UnionTest::CPrinterBaseObject* item);
	Q_INVOKABLE sdl::modsdl::UnionTest::CPrinterBaseObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::modsdl::UnionTest::CPrinterBaseObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::UnionTest::CPrinterBaseObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::modsdl::UnionTest::CPrinterBaseObject* item);
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




class CPrinterSpecificationObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::modsdl::UnionTest::PrinterSpecification, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::modsdl::UnionTest::PrinterSpecification, QVariant> BaseClass;

	CPrinterSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::modsdl::UnionTest::CPrinterSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::UnionTest::CPrinterSpecificationObjectList* otherModelPtr);
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
	CLink input;
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


class CPrinterCollectionControllerCompBase: public ::imtservergql::CObjectCollectionControllerCompBase
{

public:
	typedef ::imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CPrinterCollectionControllerCompBase)
	I_END_COMPONENT

	virtual QMap<int, QByteArray> GetSupportedCommandIds() const override;

protected:
	// reimplemented (::imtservergql::CObjectCollectionControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const override;
	virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const override;

protected:
	// Printer methods
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const CGetSpecificationsGqlRequest& getSpecificationsRequest,
				PrinterSpecification& representationPayload,
				QString& errorMessage) const = 0;

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
	virtual PrinterSpecification OnGetSpecifications(const CGetSpecificationsGqlRequest& getSpecificationsRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};




#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
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

} // namespace sdl::modsdl::UnionTest


Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CCoords::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CCoords);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterSpecificationBase::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterSpecificationBase);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CLink::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CLink);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterBase::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterBase);
