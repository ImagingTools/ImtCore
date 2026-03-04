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

// custom types includes
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace sdl::substrate::SubstrateSpecifications
{


Q_NAMESPACE

// forward declaration for union types
class SubstrateSpecificationDocument;
class CSubstrateSpecificationDocumentObject;
class CSubstrateSpecificationDocumentObjectList;

class CInputId
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
		struct InputIdFields
		{
			static const inline QString Id = "id";
		};

		istd::TSharedNullable<QByteArray> id;

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

class CCollectionViewParamsTest
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
		struct CollectionViewParamsTestFields
		{
			static const inline QString Count = "count";
			static const inline QString Offset = "offset";
		};

		istd::TSharedNullable<int> count;
		istd::TSharedNullable<int> offset;

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

class CSubstrateSpecificationDocumentTypeIds
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

class CSubstrateSpecificationListInput
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
		struct SubstrateSpecificationListInputFields
		{
			static const inline QString ViewParams = "viewParams";
		};

		istd::TSharedNullable<CCollectionViewParamsTest::V1_0> viewParams;

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

class CSubstrateSpecificationListItem
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

class CSubstrateSpecificationListResponse
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
		struct SubstrateSpecificationListResponseFields
		{
			static const inline QString Items = "items";
		};

		istd::TSharedNullable<imtsdl::TElementList<CSubstrateSpecificationListItem::V1_0>> items;

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

class CCardboardSubstrateSpecification
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
		struct CardboardSubstrateSpecificationFields
		{
			static const inline QString Color = "color";
			static const inline QString Grammage = "grammage";
		};

		istd::TSharedNullable<QString> color;
		istd::TSharedNullable<double> grammage;

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

class CPaperSubstrateSpecification
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

class CFilmSubstrateSpecification
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

class CSubstrateSpecificationInput
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

class CSubstrateSpecificationDocumentMetaInfo
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
		struct SubstrateSpecificationDocumentMetaInfoFields
		{
			static const inline QString MaterialId = "materialId";
			static const inline QString Lab = "lab";
		};

		istd::TSharedNullable<QByteArray> materialId;
		istd::TSharedNullable<QString> lab;

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





class CInputIdObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CInputId::V1_0, sdl::substrate::SubstrateSpecifications::CInputIdObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CInputId::V1_0, sdl::substrate::SubstrateSpecifications::CInputIdObject> BaseClass;

	explicit CInputIdObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CInputIdObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CInputIdObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CInputIdObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CInputIdObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CInputIdObject* item);
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





class CCollectionViewParamsTestObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTest::V1_0, sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTest::V1_0, sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject> BaseClass;

	explicit CCollectionViewParamsTestObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTestObject* item);
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





class CSubstrateSpecificationDocumentTypeIdsObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIds::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIds::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject> BaseClass;

	explicit CSubstrateSpecificationDocumentTypeIdsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIdsObject* item);
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





class CSubstrateSpecificationListInputObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInput::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInput::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject> BaseClass;

	explicit CSubstrateSpecificationListInputObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInputObject* item);
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





class CSubstrateSpecificationListItemObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItem::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItem::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject> BaseClass;

	explicit CSubstrateSpecificationListItemObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItemObject* item);
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





class CSubstrateSpecificationListResponseObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponse::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponse::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject> BaseClass;

	explicit CSubstrateSpecificationListResponseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponseObject* item);
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





class CCardboardSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecification::V1_0, sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecification::V1_0, sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject> BaseClass;

	explicit CCardboardSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecificationObject* item);
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





class CPaperSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecification::V1_0, sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecification::V1_0, sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject> BaseClass;

	explicit CPaperSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecificationObject* item);
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





class CFilmSubstrateSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecification::V1_0, sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecification::V1_0, sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject> BaseClass;

	explicit CFilmSubstrateSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecificationObject* item);
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





class CSubstrateSpecificationInputObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInput::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInput::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject> BaseClass;

	explicit CSubstrateSpecificationInputObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInputObject* item);
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





class CSubstrateSpecificationDocumentMetaInfoObjectList: public ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfo::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfo::V1_0, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject> BaseClass;

	explicit CSubstrateSpecificationDocumentMetaInfoObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfoObject* item);
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




class CSubstrateSpecificationDocumentObjectList: public ::imtsdl::TSdlAbstractListModel<sdl::substrate::SubstrateSpecifications::SubstrateSpecificationDocument, QVariant>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TSdlAbstractListModel<sdl::substrate::SubstrateSpecifications::SubstrateSpecificationDocument, QVariant> BaseClass;

	CSubstrateSpecificationDocumentObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	virtual QVariant GetOrCreateCachedObject(int index) const override;

	Q_INVOKABLE bool containsKey(const QString& nameId, int /*index*/);
	Q_INVOKABLE int getItemsCount();
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(QVariant item);
	Q_INVOKABLE sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(QVariant item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentObjectList* otherModelPtr);
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
	CSubstrateSpecificationListInput input;
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
	CInputId input;
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
	CCollectionViewParamsTest input;
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
	CSubstrateSpecificationInput input;
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
	CSubstrateSpecificationInput input;
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


class CSubstrateSpecificationCollectionControllerCompBase: public ::imtservergql::CObjectCollectionControllerCompBase
{

public:
	typedef ::imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSubstrateSpecificationCollectionControllerCompBase)
	I_END_COMPONENT

	virtual QMap<int, QByteArray> GetSupportedCommandIds() const override;

protected:
	// reimplemented (::imtservergql::CObjectCollectionControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const override;
	virtual bool SetupGqlItem(const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const override;
	virtual bool UpdateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;
	virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// SubstrateSpecification methods
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const CGetSubstrateSpecificationListGqlRequest& getSubstrateSpecificationListRequest,
				CSubstrateSpecificationListItem::V1_0& representationObject,
				QString& errorMessage) const = 0;
	virtual imtbase::ImtBaseTypes::COptionsList  OnGetOptionsList(const CGetOptionsListGqlRequest& getOptionsListGqlRequest, QString& errorMessage) const = 0;

	// SubstrateSpecification methods
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const CGetSubstrateSpecificationGqlRequest& getSubstrateSpecificationRequest,
				SubstrateSpecificationDocument& representationPayload,
				QString& errorMessage) const = 0;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const SubstrateSpecificationDocument& substrateSpecificationDocumentRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const = 0;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const CUpdateSubstrateSpecificationGqlRequest& updateSubstrateSpecificationRequest,
				istd::IChangeable& object,
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
	virtual CSubstrateSpecificationListResponse OnGetSubstrateSpecificationList(const CGetSubstrateSpecificationListGqlRequest& getSubstrateSpecificationListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual SubstrateSpecificationDocument OnGetSubstrateSpecification(const CGetSubstrateSpecificationGqlRequest& getSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual imtbase::ImtBaseTypes::COptionsList OnGetOptionsList(const CGetOptionsListGqlRequest& getOptionsListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CInputId OnInsertSubstrateSpecification(const CInsertSubstrateSpecificationGqlRequest& insertSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual CInputId OnUpdateSubstrateSpecification(const CUpdateSubstrateSpecificationGqlRequest& updateSubstrateSpecificationRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
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

} // namespace sdl::substrate::SubstrateSpecifications


Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CInputId::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CInputId);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTest::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CCollectionViewParamsTest);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIds::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentTypeIds);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInput::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListInput);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItem::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListItem);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponse::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationListResponse);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecification::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CCardboardSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecification::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CPaperSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecification::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CFilmSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInput::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationInput);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfo::V1_0);
Q_DECLARE_METATYPE(sdl::substrate::SubstrateSpecifications::CSubstrateSpecificationDocumentMetaInfo);
