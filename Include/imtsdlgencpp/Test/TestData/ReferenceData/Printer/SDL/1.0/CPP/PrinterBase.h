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


namespace sdl::V1_0::modsdl
{


// forward declaration for union types
class PrinterSpecification;
class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;
class SimpleUnion;
class CSimpleUnionObject;
class CSimpleUnionObjectList;
class MixedUnion;
class CMixedUnionObject;
class CMixedUnionObjectList;

class CPrinterSpecificationBase
{
public:
	struct PrinterSpecificationBaseFields
	{
		static const inline QString Name = "name";
	};

	istd::TSharedNullable<QString> name;

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
	};

	istd::TSharedNullable<QByteArray> link;

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
		static const inline QString Specification = "specification";
		static const inline QString SimpleTest = "simpleTest";
		static const inline QString MixedTest = "mixedTest";
	};

	istd::TSharedNullable<QString> name;
	istd::TSharedNullable<PrinterSpecification> specification;
	istd::TSharedNullable<SimpleUnion> simpleTest;
	istd::TSharedNullable<MixedUnion> mixedTest;

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

class CPrinterList
{
public:
	struct PrinterListFields
	{
		static const inline QString Data = "data";
	};

	istd::TSharedNullable<imtsdl::TElementList<CPrinterBase>> data;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CPrinterList& other) const;
	[[nodiscard]] bool operator!=(const CPrinterList& other) const {return !(operator==(other));}
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

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CLinkObject(QObject* parent = nullptr);

	QVariant GetLink();
	void SetLink(const QVariant& v);
	Q_INVOKABLE bool hasLink();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void linkChanged();

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
	Q_PROPERTY(QVariant m_specification READ GetSpecification WRITE SetSpecification NOTIFY specificationChanged)
	Q_PROPERTY(QVariant m_simpleTest READ GetSimpleTest WRITE SetSimpleTest NOTIFY simpleTestChanged)
	Q_PROPERTY(QVariant m_mixedTest READ GetMixedTest WRITE SetMixedTest NOTIFY mixedTestChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterBaseObject(QObject* parent = nullptr);

	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetSpecification();
	void SetSpecification(const QVariant& v);
	Q_INVOKABLE bool hasSpecification();
	Q_INVOKABLE void emplaceSpecification();
	void ResetSpecification();
	QVariant GetSimpleTest();
	void SetSimpleTest(const QVariant& v);
	Q_INVOKABLE bool hasSimpleTest();
	Q_INVOKABLE void emplaceSimpleTest();
	void ResetSimpleTest();
	QVariant GetMixedTest();
	void SetMixedTest(const QVariant& v);
	Q_INVOKABLE bool hasMixedTest();
	Q_INVOKABLE void emplaceMixedTest();
	void ResetMixedTest();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void nameChanged();
	void specificationChanged();
	void simpleTestChanged();
	void mixedTestChanged();

protected:
	QVariant m_specificationQObjectPtr;
	QVariant m_simpleTestQObjectPtr;
	QVariant m_mixedTestQObjectPtr;
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



class CPrinterListObjectList;

class CPrinterListObject: public ::imtbase::CItemModelBase, public CPrinterList
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_data READ GetData WRITE SetData NOTIFY dataChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterListObject(QObject* parent = nullptr);

	QVariant GetData();
	void SetData(const QVariant& v);
	Q_INVOKABLE bool hasData();
	Q_INVOKABLE void emplaceData();
	void ResetData();
	Q_INVOKABLE QVariant createDataArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataChanged();

protected:
	QVariant m_dataQObjectPtr;
};





class CPrinterListObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CPrinterList, sdl::V1_0::modsdl::CPrinterListObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CPrinterList, sdl::V1_0::modsdl::CPrinterListObject> BaseClass;

	explicit CPrinterListObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::modsdl::CPrinterListObject* item);
	Q_INVOKABLE sdl::V1_0::modsdl::CPrinterListObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::modsdl::CPrinterListObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CPrinterListObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::modsdl::CPrinterListObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};


class PrinterSpecification: public std::variant<CPrinterSpecificationBase, CLink> {

public:

	typedef std::variant<CPrinterSpecificationBase, CLink> BaseClass;

	PrinterSpecification(){};
	PrinterSpecification(const CPrinterSpecificationBase& ref)
		: BaseClass(ref){};

	PrinterSpecification(const CLink& ref)
		: BaseClass(ref){};

};



class SimpleUnion: public std::variant<QString, double> {

public:

	typedef std::variant<QString, double> BaseClass;

	SimpleUnion(){};
	SimpleUnion(const QString& ref)
		: BaseClass(ref){};

	SimpleUnion(const double& ref)
		: BaseClass(ref){};

};



class MixedUnion: public std::variant<QString, CLink> {

public:

	typedef std::variant<QString, CLink> BaseClass;

	MixedUnion(){};
	MixedUnion(const QString& ref)
		: BaseClass(ref){};

	MixedUnion(const CLink& ref)
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



class CSimpleUnionObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::modsdl::SimpleUnion, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::modsdl::SimpleUnion, QVariant> BaseClass;

	CSimpleUnionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::modsdl::CSimpleUnionObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CSimpleUnionObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};



class CMixedUnionObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::V1_0::modsdl::MixedUnion, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::V1_0::modsdl::MixedUnion, QVariant> BaseClass;

	CMixedUnionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::V1_0::modsdl::CMixedUnionObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CMixedUnionObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, QVariant item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;
signals:
	void countChanged();
};


struct GetPrintersRequestInfo
{
	bool isDataRequested = true;
		struct dataRequestInfo
		{
			bool isNameRequested = true;
			bool isSpecificationRequested = true;
			bool isSimpleTestRequested = true;
			bool isMixedTestRequested = true;
		} data;
};


struct GetPrintersRequestArguments
{
};


class CGetPrintersGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetPrintersRequestArguments& requestArguments, const GetPrintersRequestInfo& requestInfo = GetPrintersRequestInfo());

	CGetPrintersGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetPrintersRequestArguments& GetRequestedArguments() const;
	GetPrintersRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetPrintersRequestArguments m_requestedArguments;
	GetPrintersRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};




#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
{
	qmlRegisterType<CPrinterSpecificationBaseObject>("modsdlPrinterBaseSdl", 1, 0, "PrinterSpecificationBase");
	qmlRegisterType<CLinkObject>("modsdlPrinterBaseSdl", 1, 0, "Link");
	qmlRegisterType<CPrinterBaseObject>("modsdlPrinterBaseSdl", 1, 0, "PrinterBase");
	qmlRegisterType<CPrinterListObject>("modsdlPrinterBaseSdl", 1, 0, "PrinterList");
}
#endif

} // namespace sdl::V1_0::modsdl


Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CPrinterSpecificationBase);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CLink);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CPrinterBase);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CPrinterList);
