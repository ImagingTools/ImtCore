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
#include <imtbase/TListModelBase.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::imtbase::ImtBaseTypes
{


Q_NAMESPACE


class CTimeRange
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
		struct TimeRangeFields
		{
			static const inline QString Begin = "Begin";
			static const inline QString End = "End";
		};

		istd::TSharedNullable<QString> Begin;
		istd::TSharedNullable<QString> End;

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

class CSdlSize
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
		struct SdlSizeFields
		{
			static const inline QString Width = "width";
			static const inline QString Height = "height";
		};

		istd::TSharedNullable<double> width;
		istd::TSharedNullable<double> height;

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

class CSdlPoint
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
		struct SdlPointFields
		{
			static const inline QString X = "x";
			static const inline QString Y = "y";
		};

		istd::TSharedNullable<double> x;
		istd::TSharedNullable<double> y;

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

class CParamTypeIds
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
		struct ParamTypeIdsFields
		{
			static const inline QString UrlParam = "UrlParam";
			static const inline QString IdParam = "IdParam";
			static const inline QString TextParam = "TextParam";
			static const inline QString TextView = "TextView";
			static const inline QString SelectionParam = "SelectionParam";
			static const inline QString SchedulerParam = "SchedulerParam";
			static const inline QString BackupSettings = "BackupSettings";
			static const inline QString DatabaseAccessSettings = "DatabaseAccessSettings";
			static const inline QString ParamsSet = "ParamsSet";
			static const inline QString FileNameParam = "FileNameParam";
			static const inline QString IntegerParam = "IntegerParam";
			static const inline QString DoubleParam = "DoubleParam";
			static const inline QString PasswordParam = "PasswordParam";
			static const inline QString EnableableParam = "EnableableParam";
		};

		istd::TSharedNullable<QByteArray> UrlParam;
		istd::TSharedNullable<QByteArray> IdParam;
		istd::TSharedNullable<QByteArray> TextParam;
		istd::TSharedNullable<QByteArray> TextView;
		istd::TSharedNullable<QByteArray> SelectionParam;
		istd::TSharedNullable<QByteArray> SchedulerParam;
		istd::TSharedNullable<QByteArray> BackupSettings;
		istd::TSharedNullable<QByteArray> DatabaseAccessSettings;
		istd::TSharedNullable<QByteArray> ParamsSet;
		istd::TSharedNullable<QByteArray> FileNameParam;
		istd::TSharedNullable<QByteArray> IntegerParam;
		istd::TSharedNullable<QByteArray> DoubleParam;
		istd::TSharedNullable<QByteArray> PasswordParam;
		istd::TSharedNullable<QByteArray> EnableableParam;

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

class CUrlParam
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
		struct UrlParamFields
		{
			static const inline QString Scheme = "scheme";
			static const inline QString Host = "host";
			static const inline QString Port = "port";
			static const inline QString Path = "path";
		};

		istd::TSharedNullable<QString> scheme;
		istd::TSharedNullable<QString> host;
		istd::TSharedNullable<int> port;
		istd::TSharedNullable<QString> path;

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

class CIdParam
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
		struct IdParamFields
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

class CTextParam
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
		struct TextParamFields
		{
			static const inline QString Text = "text";
		};

		istd::TSharedNullable<QString> text;

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

class CEnableableParam
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
		struct EnableableParamFields
		{
			static const inline QString Value = "value";
		};

		istd::TSharedNullable<bool> value;

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

class CIntegerParam
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
		struct IntegerParamFields
		{
			static const inline QString Value = "value";
		};

		istd::TSharedNullable<int> value;

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

class CDoubleParam
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
		struct DoubleParamFields
		{
			static const inline QString Value = "value";
		};

		istd::TSharedNullable<double> value;

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

class COption
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
		struct OptionFields
		{
			static const inline QString Id = "id";
			static const inline QString Name = "name";
			static const inline QString Description = "description";
			static const inline QString Enabled = "enabled";
		};

		istd::TSharedNullable<QByteArray> id;
		istd::TSharedNullable<QString> name;
		istd::TSharedNullable<QString> description;
		istd::TSharedNullable<bool> enabled;

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

class COptionsList
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
		struct OptionsListFields
		{
			static const inline QString TotalCount = "totalCount";
			static const inline QString Offset = "offset";
			static const inline QString Options = "options";
		};

		istd::TSharedNullable<int> totalCount;
		istd::TSharedNullable<int> offset;
		istd::TSharedNullable<QList<COption::V1_0>> options;

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

class CSelectionParam
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
		struct SelectionParamFields
		{
			static const inline QString SelectedIndex = "selectedIndex";
			static const inline QString Constraints = "constraints";
		};

		istd::TSharedNullable<int> selectedIndex;
		istd::TSharedNullable<COptionsList::V1_0> constraints;

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

class CSchedulerParam
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
		struct SchedulerParamFields
		{
			static const inline QString StartTime = "startTime";
			static const inline QString Interval = "interval";
		};

		istd::TSharedNullable<QString> startTime;
		istd::TSharedNullable<int> interval;

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

class CBackupSettings
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
		struct BackupSettingsFields
		{
			static const inline QString SchedulerParam = "schedulerParam";
			static const inline QString FolderPath = "folderPath";
		};

		istd::TSharedNullable<CSchedulerParam::V1_0> schedulerParam;
		istd::TSharedNullable<QString> folderPath;

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

class CDatabaseAccessSettings
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
		struct DatabaseAccessSettingsFields
		{
			static const inline QString DbName = "dbName";
			static const inline QString Host = "host";
			static const inline QString Port = "port";
			static const inline QString DbPath = "dbPath";
			static const inline QString Username = "username";
			static const inline QString Password = "password";
		};

		istd::TSharedNullable<QString> dbName;
		istd::TSharedNullable<QString> host;
		istd::TSharedNullable<int> port;
		istd::TSharedNullable<QString> dbPath;
		istd::TSharedNullable<QString> username;
		istd::TSharedNullable<QString> password;

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

class CFileNameParam
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
		struct FileNameParamFields
		{
			static const inline QString PathType = "pathType";
			static const inline QString Path = "path";
		};

		istd::TSharedNullable<int> pathType;
		istd::TSharedNullable<QString> path;

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

class CParamsSet
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
		struct ParamsSetFields
		{
			static const inline QString ParamIds = "paramIds";
			static const inline QString ParamTypeIds = "paramTypeIds";
			static const inline QString ParamNames = "paramNames";
			static const inline QString ParamDescriptions = "paramDescriptions";
			static const inline QString Parameters = "parameters";
		};

		istd::TSharedNullable<QList<QByteArray>> paramIds;
		istd::TSharedNullable<QList<QByteArray>> paramTypeIds;
		istd::TSharedNullable<QList<QString>> paramNames;
		istd::TSharedNullable<QList<QString>> paramDescriptions;
		istd::TSharedNullable<QList<QString>> parameters;

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

class CMimeType
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
		struct MimeTypeFields
		{
			static const inline QString Type = "type";
			static const inline QString Tree = "tree";
			static const inline QString SubType = "subType";
			static const inline QString Suffix = "suffix";
			static const inline QString Parameters = "parameters";
		};

		istd::TSharedNullable<QString> type;
		istd::TSharedNullable<QList<QString>> tree;
		istd::TSharedNullable<QString> subType;
		istd::TSharedNullable<QString> suffix;
		istd::TSharedNullable<QList<QString>> parameters;

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




class CTimeRangeObjectList;

class CTimeRangeObject: public ::imtbase::CItemModelBase, public CTimeRange
{
	Q_OBJECT
	Q_PROPERTY(QString m_begin READ GetBegin WRITE SetBegin NOTIFY beginChanged)
	Q_PROPERTY(QString m_end READ GetEnd WRITE SetEnd NOTIFY endChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTimeRangeObject(QObject* parent = nullptr);

	QString GetBegin();
	void SetBegin(QString v);
	Q_INVOKABLE bool hasBegin();
	QString GetEnd();
	void SetEnd(QString v);
	Q_INVOKABLE bool hasEnd();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void beginChanged();
	void endChanged();
	void finished();

protected:
};


class CTimeRangeObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CTimeRange::V1_0, sdl::imtbase::ImtBaseTypes::CTimeRangeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CTimeRange::V1_0, sdl::imtbase::ImtBaseTypes::CTimeRangeObject> BaseClass;

	CTimeRangeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CTimeRangeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CTimeRangeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CTimeRangeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CTimeRangeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CTimeRangeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CTimeRangeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CTimeRangeObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CTimeRangeObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CTimeRangeObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CTimeRangeObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_begin"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).Begin.value());
		}
		if (nameId == "m_end"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).End.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CSdlSizeObjectList;

class CSdlSizeObject: public ::imtbase::CItemModelBase, public CSdlSize
{
	Q_OBJECT
	Q_PROPERTY(double m_width READ GetWidth WRITE SetWidth NOTIFY widthChanged)
	Q_PROPERTY(double m_height READ GetHeight WRITE SetHeight NOTIFY heightChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSdlSizeObject(QObject* parent = nullptr);

	double GetWidth();
	void SetWidth(double v);
	Q_INVOKABLE bool hasWidth();
	double GetHeight();
	void SetHeight(double v);
	Q_INVOKABLE bool hasHeight();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void widthChanged();
	void heightChanged();
	void finished();

protected:
};


class CSdlSizeObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlSize::V1_0, sdl::imtbase::ImtBaseTypes::CSdlSizeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlSize::V1_0, sdl::imtbase::ImtBaseTypes::CSdlSizeObject> BaseClass;

	CSdlSizeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSdlSizeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSdlSizeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlSizeObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CSdlSizeObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSdlSizeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSdlSizeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlSizeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSdlSizeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSdlSizeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlSizeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CSdlSizeObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSdlSizeObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSdlSizeObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSdlSizeObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSdlSizeObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CSdlSizeObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_width"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).width.value());
		}
		if (nameId == "m_height"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).height.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CSdlPointObjectList;

class CSdlPointObject: public ::imtbase::CItemModelBase, public CSdlPoint
{
	Q_OBJECT
	Q_PROPERTY(double m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(double m_y READ GetY WRITE SetY NOTIFY yChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSdlPointObject(QObject* parent = nullptr);

	double GetX();
	void SetX(double v);
	Q_INVOKABLE bool hasX();
	double GetY();
	void SetY(double v);
	Q_INVOKABLE bool hasY();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void xChanged();
	void yChanged();
	void finished();

protected:
};


class CSdlPointObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0, sdl::imtbase::ImtBaseTypes::CSdlPointObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0, sdl::imtbase::ImtBaseTypes::CSdlPointObject> BaseClass;

	CSdlPointObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSdlPointObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CSdlPointObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSdlPointObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSdlPointObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlPointObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CSdlPointObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSdlPointObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSdlPointObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlPointObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSdlPointObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSdlPointObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlPointObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSdlPointObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CSdlPointObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSdlPointObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSdlPointObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSdlPointObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSdlPointObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSdlPointObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CSdlPointObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_x"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).x.value());
		}
		if (nameId == "m_y"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).y.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CParamTypeIdsObjectList;

class CParamTypeIdsObject: public ::imtbase::CItemModelBase, public CParamTypeIds
{
	Q_OBJECT
	Q_PROPERTY(QString m_urlParam READ GetUrlParam WRITE SetUrlParam NOTIFY urlParamChanged)
	Q_PROPERTY(QString m_idParam READ GetIdParam WRITE SetIdParam NOTIFY idParamChanged)
	Q_PROPERTY(QString m_textParam READ GetTextParam WRITE SetTextParam NOTIFY textParamChanged)
	Q_PROPERTY(QString m_textView READ GetTextView WRITE SetTextView NOTIFY textViewChanged)
	Q_PROPERTY(QString m_selectionParam READ GetSelectionParam WRITE SetSelectionParam NOTIFY selectionParamChanged)
	Q_PROPERTY(QString m_schedulerParam READ GetSchedulerParam WRITE SetSchedulerParam NOTIFY schedulerParamChanged)
	Q_PROPERTY(QString m_backupSettings READ GetBackupSettings WRITE SetBackupSettings NOTIFY backupSettingsChanged)
	Q_PROPERTY(QString m_databaseAccessSettings READ GetDatabaseAccessSettings WRITE SetDatabaseAccessSettings NOTIFY databaseAccessSettingsChanged)
	Q_PROPERTY(QString m_paramsSet READ GetParamsSet WRITE SetParamsSet NOTIFY paramsSetChanged)
	Q_PROPERTY(QString m_fileNameParam READ GetFileNameParam WRITE SetFileNameParam NOTIFY fileNameParamChanged)
	Q_PROPERTY(QString m_integerParam READ GetIntegerParam WRITE SetIntegerParam NOTIFY integerParamChanged)
	Q_PROPERTY(QString m_doubleParam READ GetDoubleParam WRITE SetDoubleParam NOTIFY doubleParamChanged)
	Q_PROPERTY(QString m_passwordParam READ GetPasswordParam WRITE SetPasswordParam NOTIFY passwordParamChanged)
	Q_PROPERTY(QString m_enableableParam READ GetEnableableParam WRITE SetEnableableParam NOTIFY enableableParamChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CParamTypeIdsObject(QObject* parent = nullptr);

	QString GetUrlParam();
	void SetUrlParam(QString v);
	Q_INVOKABLE bool hasUrlParam();
	QString GetIdParam();
	void SetIdParam(QString v);
	Q_INVOKABLE bool hasIdParam();
	QString GetTextParam();
	void SetTextParam(QString v);
	Q_INVOKABLE bool hasTextParam();
	QString GetTextView();
	void SetTextView(QString v);
	Q_INVOKABLE bool hasTextView();
	QString GetSelectionParam();
	void SetSelectionParam(QString v);
	Q_INVOKABLE bool hasSelectionParam();
	QString GetSchedulerParam();
	void SetSchedulerParam(QString v);
	Q_INVOKABLE bool hasSchedulerParam();
	QString GetBackupSettings();
	void SetBackupSettings(QString v);
	Q_INVOKABLE bool hasBackupSettings();
	QString GetDatabaseAccessSettings();
	void SetDatabaseAccessSettings(QString v);
	Q_INVOKABLE bool hasDatabaseAccessSettings();
	QString GetParamsSet();
	void SetParamsSet(QString v);
	Q_INVOKABLE bool hasParamsSet();
	QString GetFileNameParam();
	void SetFileNameParam(QString v);
	Q_INVOKABLE bool hasFileNameParam();
	QString GetIntegerParam();
	void SetIntegerParam(QString v);
	Q_INVOKABLE bool hasIntegerParam();
	QString GetDoubleParam();
	void SetDoubleParam(QString v);
	Q_INVOKABLE bool hasDoubleParam();
	QString GetPasswordParam();
	void SetPasswordParam(QString v);
	Q_INVOKABLE bool hasPasswordParam();
	QString GetEnableableParam();
	void SetEnableableParam(QString v);
	Q_INVOKABLE bool hasEnableableParam();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void urlParamChanged();
	void idParamChanged();
	void textParamChanged();
	void textViewChanged();
	void selectionParamChanged();
	void schedulerParamChanged();
	void backupSettingsChanged();
	void databaseAccessSettingsChanged();
	void paramsSetChanged();
	void fileNameParamChanged();
	void integerParamChanged();
	void doubleParamChanged();
	void passwordParamChanged();
	void enableableParamChanged();
	void finished();

protected:
};


class CParamTypeIdsObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0, sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0, sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject> BaseClass;

	CParamTypeIdsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_urlParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).UrlParam.value());
		}
		if (nameId == "m_idParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).IdParam.value());
		}
		if (nameId == "m_textParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).TextParam.value());
		}
		if (nameId == "m_textView"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).TextView.value());
		}
		if (nameId == "m_selectionParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).SelectionParam.value());
		}
		if (nameId == "m_schedulerParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).SchedulerParam.value());
		}
		if (nameId == "m_backupSettings"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).BackupSettings.value());
		}
		if (nameId == "m_databaseAccessSettings"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).DatabaseAccessSettings.value());
		}
		if (nameId == "m_paramsSet"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).ParamsSet.value());
		}
		if (nameId == "m_fileNameParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).FileNameParam.value());
		}
		if (nameId == "m_integerParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).IntegerParam.value());
		}
		if (nameId == "m_doubleParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).DoubleParam.value());
		}
		if (nameId == "m_passwordParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).PasswordParam.value());
		}
		if (nameId == "m_enableableParam"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).EnableableParam.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CUrlParamObjectList;

class CUrlParamObject: public ::imtbase::CItemModelBase, public CUrlParam
{
	Q_OBJECT
	Q_PROPERTY(QString m_scheme READ GetScheme WRITE SetScheme NOTIFY schemeChanged)
	Q_PROPERTY(QString m_host READ GetHost WRITE SetHost NOTIFY hostChanged)
	Q_PROPERTY(int m_port READ GetPort WRITE SetPort NOTIFY portChanged)
	Q_PROPERTY(QString m_path READ GetPath WRITE SetPath NOTIFY pathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CUrlParamObject(QObject* parent = nullptr);

	QString GetScheme();
	void SetScheme(QString v);
	Q_INVOKABLE bool hasScheme();
	QString GetHost();
	void SetHost(QString v);
	Q_INVOKABLE bool hasHost();
	int GetPort();
	void SetPort(int v);
	Q_INVOKABLE bool hasPort();
	QString GetPath();
	void SetPath(QString v);
	Q_INVOKABLE bool hasPath();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void schemeChanged();
	void hostChanged();
	void portChanged();
	void pathChanged();
	void finished();

protected:
};


class CUrlParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0, sdl::imtbase::ImtBaseTypes::CUrlParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0, sdl::imtbase::ImtBaseTypes::CUrlParamObject> BaseClass;

	CUrlParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CUrlParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CUrlParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CUrlParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CUrlParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CUrlParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CUrlParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CUrlParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CUrlParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CUrlParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CUrlParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CUrlParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CUrlParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CUrlParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CUrlParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CUrlParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CUrlParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CUrlParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CUrlParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CUrlParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CUrlParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_scheme"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).scheme.value());
		}
		if (nameId == "m_host"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).host.value());
		}
		if (nameId == "m_port"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).port.value());
		}
		if (nameId == "m_path"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).path.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CIdParamObjectList;

class CIdParamObject: public ::imtbase::CItemModelBase, public CIdParam
{
	Q_OBJECT
	Q_PROPERTY(QString m_id READ GetId WRITE SetId NOTIFY idChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CIdParamObject(QObject* parent = nullptr);

	QString GetId();
	void SetId(QString v);
	Q_INVOKABLE bool hasId();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void idChanged();
	void finished();

protected:
};


class CIdParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CIdParam::V1_0, sdl::imtbase::ImtBaseTypes::CIdParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CIdParam::V1_0, sdl::imtbase::ImtBaseTypes::CIdParamObject> BaseClass;

	CIdParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CIdParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CIdParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CIdParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CIdParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CIdParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CIdParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CIdParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CIdParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CIdParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CIdParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CIdParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CIdParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CIdParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CIdParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CIdParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CIdParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CIdParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CIdParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CIdParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CIdParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CIdParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CIdParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CIdParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_id"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).id.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CTextParamObjectList;

class CTextParamObject: public ::imtbase::CItemModelBase, public CTextParam
{
	Q_OBJECT
	Q_PROPERTY(QString m_text READ GetText WRITE SetText NOTIFY textChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTextParamObject(QObject* parent = nullptr);

	QString GetText();
	void SetText(QString v);
	Q_INVOKABLE bool hasText();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void textChanged();
	void finished();

protected:
};


class CTextParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0, sdl::imtbase::ImtBaseTypes::CTextParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0, sdl::imtbase::ImtBaseTypes::CTextParamObject> BaseClass;

	CTextParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CTextParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CTextParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CTextParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CTextParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CTextParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CTextParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CTextParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CTextParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CTextParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CTextParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CTextParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CTextParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CTextParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CTextParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CTextParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CTextParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CTextParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CTextParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CTextParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CTextParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CTextParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CTextParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CTextParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_text"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).text.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CEnableableParamObjectList;

class CEnableableParamObject: public ::imtbase::CItemModelBase, public CEnableableParam
{
	Q_OBJECT
	Q_PROPERTY(bool m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CEnableableParamObject(QObject* parent = nullptr);

	bool GetValue();
	void SetValue(bool v);
	Q_INVOKABLE bool hasValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void valueChanged();
	void finished();

protected:
};


class CEnableableParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0, sdl::imtbase::ImtBaseTypes::CEnableableParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0, sdl::imtbase::ImtBaseTypes::CEnableableParamObject> BaseClass;

	CEnableableParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CEnableableParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CEnableableParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CEnableableParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CEnableableParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CEnableableParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CEnableableParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CEnableableParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CEnableableParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CEnableableParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CEnableableParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CEnableableParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CEnableableParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CEnableableParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CEnableableParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CEnableableParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CEnableableParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_value"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).value.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CIntegerParamObjectList;

class CIntegerParamObject: public ::imtbase::CItemModelBase, public CIntegerParam
{
	Q_OBJECT
	Q_PROPERTY(int m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CIntegerParamObject(QObject* parent = nullptr);

	int GetValue();
	void SetValue(int v);
	Q_INVOKABLE bool hasValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void valueChanged();
	void finished();

protected:
};


class CIntegerParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0, sdl::imtbase::ImtBaseTypes::CIntegerParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0, sdl::imtbase::ImtBaseTypes::CIntegerParamObject> BaseClass;

	CIntegerParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CIntegerParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CIntegerParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CIntegerParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CIntegerParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CIntegerParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CIntegerParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CIntegerParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CIntegerParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CIntegerParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CIntegerParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CIntegerParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CIntegerParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CIntegerParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CIntegerParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CIntegerParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CIntegerParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_value"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).value.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CDoubleParamObjectList;

class CDoubleParamObject: public ::imtbase::CItemModelBase, public CDoubleParam
{
	Q_OBJECT
	Q_PROPERTY(double m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDoubleParamObject(QObject* parent = nullptr);

	double GetValue();
	void SetValue(double v);
	Q_INVOKABLE bool hasValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void valueChanged();
	void finished();

protected:
};


class CDoubleParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0, sdl::imtbase::ImtBaseTypes::CDoubleParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0, sdl::imtbase::ImtBaseTypes::CDoubleParamObject> BaseClass;

	CDoubleParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CDoubleParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CDoubleParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CDoubleParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CDoubleParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CDoubleParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CDoubleParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CDoubleParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CDoubleParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CDoubleParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CDoubleParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CDoubleParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CDoubleParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CDoubleParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CDoubleParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CDoubleParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CDoubleParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_value"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).value.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class COptionObjectList;

class COptionObject: public ::imtbase::CItemModelBase, public COption
{
	Q_OBJECT
	Q_PROPERTY(QString m_id READ GetId WRITE SetId NOTIFY idChanged)
	Q_PROPERTY(QString m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QString m_description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(bool m_enabled READ GetEnabled WRITE SetEnabled NOTIFY enabledChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COptionObject(QObject* parent = nullptr);

	QString GetId();
	void SetId(QString v);
	Q_INVOKABLE bool hasId();
	QString GetName();
	void SetName(QString v);
	Q_INVOKABLE bool hasName();
	QString GetDescription();
	void SetDescription(QString v);
	Q_INVOKABLE bool hasDescription();
	bool GetEnabled();
	void SetEnabled(bool v);
	Q_INVOKABLE bool hasEnabled();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void idChanged();
	void nameChanged();
	void descriptionChanged();
	void enabledChanged();
	void finished();

protected:
};


class COptionObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::COption::V1_0, sdl::imtbase::ImtBaseTypes::COptionObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::COption::V1_0, sdl::imtbase::ImtBaseTypes::COptionObject> BaseClass;

	COptionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::COptionObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::COptionObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::COptionObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::COptionObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::COptionObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::COptionObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::COptionObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::COptionObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::COptionObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::COptionObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::COptionObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::COptionObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::COptionObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::COptionObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::COptionObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::COptionObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::COptionObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::COptionObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::COptionObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::COptionObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_id"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).id.value());
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).name.value());
		}
		if (nameId == "m_description"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).description.value());
		}
		if (nameId == "m_enabled"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).enabled.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class COptionsListObjectList;

class COptionsListObject: public ::imtbase::CItemModelBase, public COptionsList
{
	Q_OBJECT
	Q_PROPERTY(int m_totalCount READ GetTotalCount WRITE SetTotalCount NOTIFY totalCountChanged)
	Q_PROPERTY(int m_offset READ GetOffset WRITE SetOffset NOTIFY offsetChanged)
	Q_PROPERTY(sdl::imtbase::ImtBaseTypes::COptionObjectList* m_options READ GetOptions WRITE SetOptions NOTIFY optionsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COptionsListObject(QObject* parent = nullptr);

	int GetTotalCount();
	void SetTotalCount(int v);
	Q_INVOKABLE bool hasTotalCount();
	int GetOffset();
	void SetOffset(int v);
	Q_INVOKABLE bool hasOffset();
	sdl::imtbase::ImtBaseTypes::COptionObjectList* GetOptions();
	void SetOptions(sdl::imtbase::ImtBaseTypes::COptionObjectList* v);
	Q_INVOKABLE bool hasOptions();
	Q_INVOKABLE void createOptions();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void totalCountChanged();
	void offsetChanged();
	void optionsChanged();
	void finished();

protected:
	sdl::imtbase::ImtBaseTypes::COptionObjectList* m_optionsQObjectPtr;
};


class COptionsListObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::COptionsList::V1_0, sdl::imtbase::ImtBaseTypes::COptionsListObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::COptionsList::V1_0, sdl::imtbase::ImtBaseTypes::COptionsListObject> BaseClass;

	COptionsListObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::COptionsListObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::COptionsListObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::COptionsListObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::COptionsListObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::COptionsListObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::COptionsListObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::COptionsListObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::COptionsListObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::COptionsListObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::COptionsListObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::COptionsListObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::COptionsListObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::COptionsListObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::COptionsListObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::COptionsListObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::COptionsListObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::COptionsListObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::COptionsListObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_totalCount"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).totalCount.value());
		}
		if (nameId == "m_offset"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).offset.value());
		}
		if (nameId == "m_options"){
			sdl::imtbase::ImtBaseTypes::COptionsListObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetOptions());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CSelectionParamObjectList;

class CSelectionParamObject: public ::imtbase::CItemModelBase, public CSelectionParam
{
	Q_OBJECT
	Q_PROPERTY(int m_selectedIndex READ GetSelectedIndex WRITE SetSelectedIndex NOTIFY selectedIndexChanged)
	Q_PROPERTY(sdl::imtbase::ImtBaseTypes::COptionsListObject* m_constraints READ GetConstraints WRITE SetConstraints NOTIFY constraintsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSelectionParamObject(QObject* parent = nullptr);

	int GetSelectedIndex();
	void SetSelectedIndex(int v);
	Q_INVOKABLE bool hasSelectedIndex();
	sdl::imtbase::ImtBaseTypes::COptionsListObject* GetConstraints();
	void SetConstraints(sdl::imtbase::ImtBaseTypes::COptionsListObject* v);
	Q_INVOKABLE bool hasConstraints();
	Q_INVOKABLE void createConstraints();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void selectedIndexChanged();
	void constraintsChanged();
	void finished();

protected:
	sdl::imtbase::ImtBaseTypes::COptionsListObject* m_constraintsQObjectPtr;
};


class CSelectionParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0, sdl::imtbase::ImtBaseTypes::CSelectionParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0, sdl::imtbase::ImtBaseTypes::CSelectionParamObject> BaseClass;

	CSelectionParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSelectionParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSelectionParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSelectionParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CSelectionParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSelectionParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSelectionParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSelectionParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSelectionParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSelectionParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSelectionParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CSelectionParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSelectionParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSelectionParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSelectionParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSelectionParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CSelectionParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_selectedIndex"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).selectedIndex.value());
		}
		if (nameId == "m_constraints"){
			sdl::imtbase::ImtBaseTypes::CSelectionParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetConstraints());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CSchedulerParamObjectList;

class CSchedulerParamObject: public ::imtbase::CItemModelBase, public CSchedulerParam
{
	Q_OBJECT
	Q_PROPERTY(QString m_startTime READ GetStartTime WRITE SetStartTime NOTIFY startTimeChanged)
	Q_PROPERTY(int m_interval READ GetInterval WRITE SetInterval NOTIFY intervalChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSchedulerParamObject(QObject* parent = nullptr);

	QString GetStartTime();
	void SetStartTime(QString v);
	Q_INVOKABLE bool hasStartTime();
	int GetInterval();
	void SetInterval(int v);
	Q_INVOKABLE bool hasInterval();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void startTimeChanged();
	void intervalChanged();
	void finished();

protected:
};


class CSchedulerParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0, sdl::imtbase::ImtBaseTypes::CSchedulerParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0, sdl::imtbase::ImtBaseTypes::CSchedulerParamObject> BaseClass;

	CSchedulerParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_startTime"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).startTime.value());
		}
		if (nameId == "m_interval"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).interval.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CBackupSettingsObjectList;

class CBackupSettingsObject: public ::imtbase::CItemModelBase, public CBackupSettings
{
	Q_OBJECT
	Q_PROPERTY(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* m_schedulerParam READ GetSchedulerParam WRITE SetSchedulerParam NOTIFY schedulerParamChanged)
	Q_PROPERTY(QString m_folderPath READ GetFolderPath WRITE SetFolderPath NOTIFY folderPathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CBackupSettingsObject(QObject* parent = nullptr);

	sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* GetSchedulerParam();
	void SetSchedulerParam(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* v);
	Q_INVOKABLE bool hasSchedulerParam();
	Q_INVOKABLE void createSchedulerParam();
	QString GetFolderPath();
	void SetFolderPath(QString v);
	Q_INVOKABLE bool hasFolderPath();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void schedulerParamChanged();
	void folderPathChanged();
	void finished();

protected:
	sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* m_schedulerParamQObjectPtr;
};


class CBackupSettingsObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CBackupSettings::V1_0, sdl::imtbase::ImtBaseTypes::CBackupSettingsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CBackupSettings::V1_0, sdl::imtbase::ImtBaseTypes::CBackupSettingsObject> BaseClass;

	CBackupSettingsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_schedulerParam"){
			sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetSchedulerParam());
		}
		if (nameId == "m_folderPath"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).folderPath.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CDatabaseAccessSettingsObjectList;

class CDatabaseAccessSettingsObject: public ::imtbase::CItemModelBase, public CDatabaseAccessSettings
{
	Q_OBJECT
	Q_PROPERTY(QString m_dbName READ GetDbName WRITE SetDbName NOTIFY dbNameChanged)
	Q_PROPERTY(QString m_host READ GetHost WRITE SetHost NOTIFY hostChanged)
	Q_PROPERTY(int m_port READ GetPort WRITE SetPort NOTIFY portChanged)
	Q_PROPERTY(QString m_dbPath READ GetDbPath WRITE SetDbPath NOTIFY dbPathChanged)
	Q_PROPERTY(QString m_username READ GetUsername WRITE SetUsername NOTIFY usernameChanged)
	Q_PROPERTY(QString m_password READ GetPassword WRITE SetPassword NOTIFY passwordChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDatabaseAccessSettingsObject(QObject* parent = nullptr);

	QString GetDbName();
	void SetDbName(QString v);
	Q_INVOKABLE bool hasDbName();
	QString GetHost();
	void SetHost(QString v);
	Q_INVOKABLE bool hasHost();
	int GetPort();
	void SetPort(int v);
	Q_INVOKABLE bool hasPort();
	QString GetDbPath();
	void SetDbPath(QString v);
	Q_INVOKABLE bool hasDbPath();
	QString GetUsername();
	void SetUsername(QString v);
	Q_INVOKABLE bool hasUsername();
	QString GetPassword();
	void SetPassword(QString v);
	Q_INVOKABLE bool hasPassword();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dbNameChanged();
	void hostChanged();
	void portChanged();
	void dbPathChanged();
	void usernameChanged();
	void passwordChanged();
	void finished();

protected:
};


class CDatabaseAccessSettingsObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings::V1_0, sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings::V1_0, sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject> BaseClass;

	CDatabaseAccessSettingsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_dbName"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dbName.value());
		}
		if (nameId == "m_host"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).host.value());
		}
		if (nameId == "m_port"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).port.value());
		}
		if (nameId == "m_dbPath"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).dbPath.value());
		}
		if (nameId == "m_username"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).username.value());
		}
		if (nameId == "m_password"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).password.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CFileNameParamObjectList;

class CFileNameParamObject: public ::imtbase::CItemModelBase, public CFileNameParam
{
	Q_OBJECT
	Q_PROPERTY(int m_pathType READ GetPathType WRITE SetPathType NOTIFY pathTypeChanged)
	Q_PROPERTY(QString m_path READ GetPath WRITE SetPath NOTIFY pathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFileNameParamObject(QObject* parent = nullptr);

	int GetPathType();
	void SetPathType(int v);
	Q_INVOKABLE bool hasPathType();
	QString GetPath();
	void SetPath(QString v);
	Q_INVOKABLE bool hasPath();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void pathTypeChanged();
	void pathChanged();
	void finished();

protected:
};


class CFileNameParamObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0, sdl::imtbase::ImtBaseTypes::CFileNameParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0, sdl::imtbase::ImtBaseTypes::CFileNameParamObject> BaseClass;

	CFileNameParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CFileNameParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CFileNameParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CFileNameParamObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CFileNameParamObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CFileNameParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CFileNameParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CFileNameParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CFileNameParamObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CFileNameParamObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CFileNameParamObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CFileNameParamObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CFileNameParamObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CFileNameParamObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CFileNameParamObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CFileNameParamObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CFileNameParamObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_pathType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).pathType.value());
		}
		if (nameId == "m_path"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).path.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CParamsSetObjectList;

class CParamsSetObject: public ::imtbase::CItemModelBase, public CParamsSet
{
	Q_OBJECT
	Q_PROPERTY(QList<QString> m_paramIds READ GetParamIds WRITE SetParamIds NOTIFY paramIdsChanged)
	Q_PROPERTY(QList<QString> m_paramTypeIds READ GetParamTypeIds WRITE SetParamTypeIds NOTIFY paramTypeIdsChanged)
	Q_PROPERTY(QList<QString> m_paramNames READ GetParamNames WRITE SetParamNames NOTIFY paramNamesChanged)
	Q_PROPERTY(QList<QString> m_paramDescriptions READ GetParamDescriptions WRITE SetParamDescriptions NOTIFY paramDescriptionsChanged)
	Q_PROPERTY(QList<QString> m_parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CParamsSetObject(QObject* parent = nullptr);

	QList<QString> GetParamIds();
	void SetParamIds(QList<QString> v);
	Q_INVOKABLE bool hasParamIds();
	QList<QString> GetParamTypeIds();
	void SetParamTypeIds(QList<QString> v);
	Q_INVOKABLE bool hasParamTypeIds();
	QList<QString> GetParamNames();
	void SetParamNames(QList<QString> v);
	Q_INVOKABLE bool hasParamNames();
	QList<QString> GetParamDescriptions();
	void SetParamDescriptions(QList<QString> v);
	Q_INVOKABLE bool hasParamDescriptions();
	QList<QString> GetParameters();
	void SetParameters(QList<QString> v);
	Q_INVOKABLE bool hasParameters();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void paramIdsChanged();
	void paramTypeIdsChanged();
	void paramNamesChanged();
	void paramDescriptionsChanged();
	void parametersChanged();
	void finished();

protected:
};


class CParamsSetObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0, sdl::imtbase::ImtBaseTypes::CParamsSetObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0, sdl::imtbase::ImtBaseTypes::CParamsSetObject> BaseClass;

	CParamsSetObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CParamsSetObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CParamsSetObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CParamsSetObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CParamsSetObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamsSetObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CParamsSetObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CParamsSetObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CParamsSetObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamsSetObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CParamsSetObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CParamsSetObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamsSetObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CParamsSetObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CParamsSetObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CParamsSetObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CParamsSetObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CParamsSetObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CParamsSetObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CParamsSetObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CParamsSetObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_paramIds"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).paramIds.value());
		}
		if (nameId == "m_paramTypeIds"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).paramTypeIds.value());
		}
		if (nameId == "m_paramNames"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).paramNames.value());
		}
		if (nameId == "m_paramDescriptions"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).paramDescriptions.value());
		}
		if (nameId == "m_parameters"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).parameters.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CMimeTypeObjectList;

class CMimeTypeObject: public ::imtbase::CItemModelBase, public CMimeType
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType WRITE SetType NOTIFY typeChanged)
	Q_PROPERTY(QList<QString> m_tree READ GetTree WRITE SetTree NOTIFY treeChanged)
	Q_PROPERTY(QString m_subType READ GetSubType WRITE SetSubType NOTIFY subTypeChanged)
	Q_PROPERTY(QString m_suffix READ GetSuffix WRITE SetSuffix NOTIFY suffixChanged)
	Q_PROPERTY(QList<QString> m_parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CMimeTypeObject(QObject* parent = nullptr);

	QString GetType();
	void SetType(QString v);
	Q_INVOKABLE bool hasType();
	QList<QString> GetTree();
	void SetTree(QList<QString> v);
	Q_INVOKABLE bool hasTree();
	QString GetSubType();
	void SetSubType(QString v);
	Q_INVOKABLE bool hasSubType();
	QString GetSuffix();
	void SetSuffix(QString v);
	Q_INVOKABLE bool hasSuffix();
	QList<QString> GetParameters();
	void SetParameters(QList<QString> v);
	Q_INVOKABLE bool hasParameters();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void typeChanged();
	void treeChanged();
	void subTypeChanged();
	void suffixChanged();
	void parametersChanged();
	void finished();

protected:
};


class CMimeTypeObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CMimeType::V1_0, sdl::imtbase::ImtBaseTypes::CMimeTypeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ImtBaseTypes::CMimeType::V1_0, sdl::imtbase::ImtBaseTypes::CMimeTypeObject> BaseClass;

	CMimeTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* copyMe(){
		sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* objectListPtr = new sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CMimeTypeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CMimeTypeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CMimeTypeObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ImtBaseTypes::CMimeTypeObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CMimeTypeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CMimeTypeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CMimeTypeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ImtBaseTypes::CMimeTypeObject>()){
				return nullptr;
			}

			sdl::imtbase::ImtBaseTypes::CMimeTypeObject* itemObjectPtr = item.value<sdl::imtbase::ImtBaseTypes::CMimeTypeObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::imtbase::ImtBaseTypes::CMimeTypeObject>()){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CMimeTypeObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CMimeTypeObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CMimeTypeObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ImtBaseTypes::CMimeTypeObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::imtbase::ImtBaseTypes::CMimeTypeObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_type"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).type.value());
		}
		if (nameId == "m_tree"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).tree.value());
		}
		if (nameId == "m_subType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).subType.value());
		}
		if (nameId == "m_suffix"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).suffix.value());
		}
		if (nameId == "m_parameters"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).parameters.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};
#ifdef QT_QML_LIB
static void RegisterQmlTypes()
{
	qmlRegisterType<CTimeRangeObject>("imtbaseImtBaseTypesSdl", 1, 0, "TimeRange");
	qmlRegisterType<CSdlSizeObject>("imtbaseImtBaseTypesSdl", 1, 0, "SdlSize");
	qmlRegisterType<CSdlPointObject>("imtbaseImtBaseTypesSdl", 1, 0, "SdlPoint");
	qmlRegisterType<CParamTypeIdsObject>("imtbaseImtBaseTypesSdl", 1, 0, "ParamTypeIds");
	qmlRegisterType<CUrlParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "UrlParam");
	qmlRegisterType<CIdParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "IdParam");
	qmlRegisterType<CTextParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "TextParam");
	qmlRegisterType<CEnableableParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "EnableableParam");
	qmlRegisterType<CIntegerParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "IntegerParam");
	qmlRegisterType<CDoubleParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "DoubleParam");
	qmlRegisterType<COptionObject>("imtbaseImtBaseTypesSdl", 1, 0, "Option");
	qmlRegisterType<COptionsListObject>("imtbaseImtBaseTypesSdl", 1, 0, "OptionsList");
	qmlRegisterType<CSelectionParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "SelectionParam");
	qmlRegisterType<CSchedulerParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "SchedulerParam");
	qmlRegisterType<CBackupSettingsObject>("imtbaseImtBaseTypesSdl", 1, 0, "BackupSettings");
	qmlRegisterType<CDatabaseAccessSettingsObject>("imtbaseImtBaseTypesSdl", 1, 0, "DatabaseAccessSettings");
	qmlRegisterType<CFileNameParamObject>("imtbaseImtBaseTypesSdl", 1, 0, "FileNameParam");
	qmlRegisterType<CParamsSetObject>("imtbaseImtBaseTypesSdl", 1, 0, "ParamsSet");
	qmlRegisterType<CMimeTypeObject>("imtbaseImtBaseTypesSdl", 1, 0, "MimeType");
}
#endif

} // namespace sdl::imtbase::ImtBaseTypes


Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CTimeRange::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CTimeRange);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSdlSize::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSdlSize);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSdlPoint);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CParamTypeIds);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CUrlParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CIdParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CIdParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CTextParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CTextParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CEnableableParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CIntegerParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CDoubleParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::COption::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::COption);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::COptionsList::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::COptionsList);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSelectionParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CSchedulerParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CBackupSettings::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CBackupSettings);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CFileNameParam);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CParamsSet);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CMimeType::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ImtBaseTypes::CMimeType);
