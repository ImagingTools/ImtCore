#pragma once

#include "SubstrateSpecifications_fwd.h"

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

// custom types includes
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace sdl::V1_0::substrate
{


inline namespace SubstrateSpecificationsSdlEnums
{



Q_NAMESPACE


} // inline namespace SubstrateSpecificationsSdlEnums

// forward declaration for union types
class SubstrateSpecificationDocument;

class CInputId
{
public:
	struct InputIdFields
	{
		static const inline QString Id = "id";
	};

	istd::TNullableValue<QByteArray> id;

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

	istd::TNullableValue<int> count;
	istd::TNullableValue<int> offset;

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

	istd::TNullableValue<QByteArray> PaperID;
	istd::TNullableValue<QByteArray> CardboardID;
	istd::TNullableValue<QByteArray> FilmID;

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

	istd::TNullableValue<CCollectionViewParamsTest> viewParams;

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

	istd::TNullableValue<QByteArray> id;
	istd::TNullableValue<QByteArray> typeId;
	istd::TNullableValue<QString> name;
	istd::TNullableValue<QByteArray> materialId;
	istd::TNullableValue<QString> materialName;
	istd::TNullableValue<QString> added;
	istd::TNullableValue<QString> timeStamp;
	istd::TNullableValue<QString> colorPreview;

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

	istd::TNullableValue<imtsdl::TElementList<CSubstrateSpecificationListItem>> items;

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

	istd::TNullableValue<QString> color;
	istd::TNullableValue<double> grammage;

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

	istd::TNullableValue<double> grammage;
	istd::TNullableValue<QString> color;
	istd::TNullableValue<QString> coatType;

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

	istd::TNullableValue<double> transparency;
	istd::TNullableValue<QString> color;
	istd::TNullableValue<QString> material;

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

	istd::TNullableValue<QByteArray> id;
	istd::TNullableValue<QString> name;
	istd::TNullableValue<QString> description;
	istd::TNullableValue<SubstrateSpecificationDocument> item;
	istd::TNullableValue<QByteArray> typeId;

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

	istd::TNullableValue<QByteArray> materialId;
	istd::TNullableValue<QString> lab;

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
	istd::TNullableValue<V1_0::substrate::CSubstrateSpecificationListInput> input;
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
	istd::TNullableValue<V1_0::substrate::CInputId> input;
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
	istd::TNullableValue<V1_0::substrate::CCollectionViewParamsTest> input;
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
	istd::TNullableValue<V1_0::substrate::CSubstrateSpecificationInput> input;
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
	istd::TNullableValue<V1_0::substrate::CSubstrateSpecificationInput> input;
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


} // namespace sdl::V1_0::substrate


Q_DECLARE_METATYPE(sdl::V1_0::substrate::CInputId);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CCollectionViewParamsTest);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CSubstrateSpecificationDocumentTypeIds);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CSubstrateSpecificationListInput);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CSubstrateSpecificationListItem);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CSubstrateSpecificationListResponse);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CCardboardSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CPaperSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CFilmSubstrateSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CSubstrateSpecificationInput);
Q_DECLARE_METATYPE(sdl::V1_0::substrate::CSubstrateSpecificationDocumentMetaInfo);
