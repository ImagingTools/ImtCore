/// \file PrinterSpecification.h

#pragma once

namespace sdl::modsdl::UnionTest
{

class PrinterSpecification;

} // namespace sdl::modsdl::UnionTest




/// \file CCoords.h

#pragma once


// C/C++ includes
#include <optional>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>



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

		std::optional<double> X;
		std::optional<double> Y;

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
	std::optional<V1_0> Version_1_0;

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


// C/C++ includes
#include <optional>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>



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

		std::optional<QString> name;

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
	std::optional<V1_0> Version_1_0;

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


// C/C++ includes
#include <optional>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>



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

		std::optional<QByteArray> link;

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
	std::optional<V1_0> Version_1_0;

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


// C/C++ includes
#include <optional>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>



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

		std::optional<QString> name;
		std::optional<std::shared_ptr<PrinterSpecification>> specification;

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
	std::optional<V1_0> Version_1_0;

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
} // namespace sdl::modsdl::UnionTest




