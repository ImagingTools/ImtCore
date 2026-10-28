#pragma once

#include "UnionTest_fwd.h"

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
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
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


} // namespace sdl::V1_0::modsdl


Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CCoords);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CPrinterSpecificationBase);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CLink);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CPrinterBase);
