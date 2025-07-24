/// \file PrinterSpecification.h

#pragma once

namespace sdl::modsdl::UnionTest
{

class PrinterSpecification;
class CPrinterSpecificationObject;

} // namespace sdl::modsdl::UnionTest




/// \file CCoords.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::modsdl::UnionTest
{


class CCoords
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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


} // namespace sdl::modsdl::UnionTest


Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CCoords::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CCoords);





/// \file CPrinterSpecificationBase.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::modsdl::UnionTest
{


class CPrinterSpecificationBase
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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


} // namespace sdl::modsdl::UnionTest


Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterSpecificationBase::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterSpecificationBase);





/// \file CLink.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::modsdl::UnionTest
{


class CLink
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct LinkFields
		{
			static const inline QString Link = "link";
		};

		istd::TSharedNullable<QByteArray> link;

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


} // namespace sdl::modsdl::UnionTest


Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CLink::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CLink);





/// \file CPrinterBase.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::modsdl::UnionTest
{


class CPrinterBase
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct PrinterBaseFields
		{
			static const inline QString Name = "name";
			static const inline QString Specification = "specification";
		};

		istd::TSharedNullable<QString> name;
		istd::TSharedNullable<std::shared_ptr<PrinterSpecification>> specification;

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


} // namespace sdl::modsdl::UnionTest


Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterBase::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::UnionTest::CPrinterBase);





/// \file PrinterSpecification_ClassDef.h

#pragma once

// STD includes
#include <variant>



namespace sdl::modsdl::UnionTest
{


class PrinterSpecification: public std::variant<CPrinterSpecificationBase, CLink> {

public:

	typedef std::variant<CPrinterSpecificationBase, CLink> BaseClass;

	PrinterSpecification(){};
	PrinterSpecification(const CPrinterSpecificationBase& ref)
		: BaseClass(ref){};

	PrinterSpecification(const CLink& ref)
		: BaseClass(ref){};

};

class CPrinterSpecificationObject: public ::imtbase::CItemModelBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

public:
	typedef ::imtbase::CItemModelBase BaseClass;

	CPrinterSpecificationObject(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE QString GetType() const{
		return m_type;
	}

	Q_INVOKABLE void SetValue(const QVariant& value){
		if (value.canConvert<CType1>()){
			m_type = "CPrinterSpecificationBase";
		}

		if (value.canConvert<CType1>()){
			m_type = "CLink";
		}

		m_value = value;
	}

	Q_INVOKABLE QVariant GetValue(){
		if (const CPrinterSpecificationBase* val = std::get_if<CPrinterSpecificationBase>((*Version_1_0).get())){
			if (!m_cprinterspecificationbaseObject){
				m_cprinterspecificationbaseObject= new CPrinterSpecificationBaseObject(this);
			}

			return QVariant::fromValue(m_cprinterspecificationbaseObject);
		}

		if (const CLink* val = std::get_if<CLink>((*Version_1_0).get())){
			if (!m_clinkObject){
				m_clinkObject= new CLinkObject(this);
			}

			return QVariant::fromValue(m_clinkObject);
		}

		return QVariant();
	}

signals:
	void typeChanged();
	void valueChanged();

public:
	istd::TSharedNullable<std::shared_ptr<PrinterSpecification>> Version_1_0;
	CPrinterSpecificationBaseObject* m_cprinterspecificationbaseObject;
	CLinkObject* m_clinkObject;
	QVariant m_value;
	QString m_type;
};

} // namespace sdl::modsdl::UnionTest




/// \file CGetSpecificationsGqlRequest.h

#pragma once


// Qt includes
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>


// imtgql includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlParamObject.h>


namespace sdl::modsdl::UnionTest
{



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


} // namespace sdl::modsdl::UnionTest





/// \file CGraphQlHandlerCompBase.h

#pragma once

//  includes
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>



namespace sdl::modsdl::UnionTest
{


class CGraphQlHandlerCompBase:
			virtual public ::imtservergql::CPermissibleGqlRequestHandlerComp
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
	virtual std::shared_ptr<PrinterSpecification> OnGetSpecifications(const CGetSpecificationsGqlRequest& getSpecificationsRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};


} // namespace sdl::modsdl::UnionTest



/// \file CPrinterCollectionControllerCompBase.h

#pragma once

//  includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>



namespace sdl::modsdl::UnionTest
{


class CPrinterCollectionControllerCompBase:
			virtual public ::imtservergql::CObjectCollectionControllerCompBase
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
				std::shared_ptr<PrinterSpecification>& representationPayload,
				QString& errorMessage) const = 0;

};


} // namespace sdl::modsdl::UnionTest



