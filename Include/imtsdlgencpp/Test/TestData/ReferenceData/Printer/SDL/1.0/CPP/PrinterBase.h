#pragma once

#include "PrinterBase_fwd.h"

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


inline namespace PrinterBaseSdlEnums
{



Q_NAMESPACE


} // inline namespace PrinterBaseSdlEnums

// forward declaration for union types
class PrinterSpecification;
class SimpleUnion;
class MixedUnion;

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
	};

	istd::TNullableValue<QByteArray> link;

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

	istd::TNullableValue<QString> name;
	istd::TNullableValue<PrinterSpecification> specification;
	istd::TNullableValue<SimpleUnion> simpleTest;
	istd::TNullableValue<MixedUnion> mixedTest;

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

	istd::TNullableValue<imtsdl::TElementList<CPrinterBase>> data;

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


} // namespace sdl::V1_0::modsdl


Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CPrinterSpecificationBase);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CLink);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CPrinterBase);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CPrinterList);
