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
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::imtbase::ImtBaseTypes
{


Q_NAMESPACE


class CTimeRange
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		istd::TSharedNullable<imtsdl::TElementList<COption::V1_0>> options;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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

		istd::TSharedNullable<imtsdl::TElementList<QByteArray>> paramIds;
		istd::TSharedNullable<imtsdl::TElementList<QByteArray>> paramTypeIds;
		istd::TSharedNullable<imtsdl::TElementList<QString>> paramNames;
		istd::TSharedNullable<imtsdl::TElementList<QString>> paramDescriptions;
		istd::TSharedNullable<imtsdl::TElementList<QString>> parameters;

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
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
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
		istd::TSharedNullable<imtsdl::TElementList<QString>> tree;
		istd::TSharedNullable<QString> subType;
		istd::TSharedNullable<QString> suffix;
		istd::TSharedNullable<imtsdl::TElementList<QString>> parameters;

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
	Q_PROPERTY(QVariant m_begin READ GetBegin WRITE SetBegin NOTIFY beginChanged)
	Q_PROPERTY(QVariant m_end READ GetEnd WRITE SetEnd NOTIFY endChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTimeRangeObject(QObject* parent = nullptr);

	QVariant GetBegin();
	void SetBegin(const QVariant& v);
	Q_INVOKABLE bool hasBegin();
	QVariant GetEnd();
	void SetEnd(const QVariant& v);
	Q_INVOKABLE bool hasEnd();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void beginChanged();
	void endChanged();

protected:
};





class CTimeRangeObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CTimeRange::V1_0, sdl::imtbase::ImtBaseTypes::CTimeRangeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CTimeRange::V1_0, sdl::imtbase::ImtBaseTypes::CTimeRangeObject> BaseClass;

	explicit CTimeRangeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSdlSizeObjectList;

class CSdlSizeObject: public ::imtbase::CItemModelBase, public CSdlSize
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_width READ GetWidth WRITE SetWidth NOTIFY widthChanged)
	Q_PROPERTY(QVariant m_height READ GetHeight WRITE SetHeight NOTIFY heightChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSdlSizeObject(QObject* parent = nullptr);

	QVariant GetWidth();
	void SetWidth(const QVariant& v);
	Q_INVOKABLE bool hasWidth();
	QVariant GetHeight();
	void SetHeight(const QVariant& v);
	Q_INVOKABLE bool hasHeight();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void widthChanged();
	void heightChanged();

protected:
};





class CSdlSizeObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlSize::V1_0, sdl::imtbase::ImtBaseTypes::CSdlSizeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlSize::V1_0, sdl::imtbase::ImtBaseTypes::CSdlSizeObject> BaseClass;

	explicit CSdlSizeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSdlPointObjectList;

class CSdlPointObject: public ::imtbase::CItemModelBase, public CSdlPoint
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(QVariant m_y READ GetY WRITE SetY NOTIFY yChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSdlPointObject(QObject* parent = nullptr);

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





class CSdlPointObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0, sdl::imtbase::ImtBaseTypes::CSdlPointObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSdlPoint::V1_0, sdl::imtbase::ImtBaseTypes::CSdlPointObject> BaseClass;

	explicit CSdlPointObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSdlPointObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSdlPointObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSdlPointObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CParamTypeIdsObjectList;

class CParamTypeIdsObject: public ::imtbase::CItemModelBase, public CParamTypeIds
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_urlParam READ GetUrlParam WRITE SetUrlParam NOTIFY urlParamChanged)
	Q_PROPERTY(QVariant m_idParam READ GetIdParam WRITE SetIdParam NOTIFY idParamChanged)
	Q_PROPERTY(QVariant m_textParam READ GetTextParam WRITE SetTextParam NOTIFY textParamChanged)
	Q_PROPERTY(QVariant m_textView READ GetTextView WRITE SetTextView NOTIFY textViewChanged)
	Q_PROPERTY(QVariant m_selectionParam READ GetSelectionParam WRITE SetSelectionParam NOTIFY selectionParamChanged)
	Q_PROPERTY(QVariant m_schedulerParam READ GetSchedulerParam WRITE SetSchedulerParam NOTIFY schedulerParamChanged)
	Q_PROPERTY(QVariant m_backupSettings READ GetBackupSettings WRITE SetBackupSettings NOTIFY backupSettingsChanged)
	Q_PROPERTY(QVariant m_databaseAccessSettings READ GetDatabaseAccessSettings WRITE SetDatabaseAccessSettings NOTIFY databaseAccessSettingsChanged)
	Q_PROPERTY(QVariant m_paramsSet READ GetParamsSet WRITE SetParamsSet NOTIFY paramsSetChanged)
	Q_PROPERTY(QVariant m_fileNameParam READ GetFileNameParam WRITE SetFileNameParam NOTIFY fileNameParamChanged)
	Q_PROPERTY(QVariant m_integerParam READ GetIntegerParam WRITE SetIntegerParam NOTIFY integerParamChanged)
	Q_PROPERTY(QVariant m_doubleParam READ GetDoubleParam WRITE SetDoubleParam NOTIFY doubleParamChanged)
	Q_PROPERTY(QVariant m_passwordParam READ GetPasswordParam WRITE SetPasswordParam NOTIFY passwordParamChanged)
	Q_PROPERTY(QVariant m_enableableParam READ GetEnableableParam WRITE SetEnableableParam NOTIFY enableableParamChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CParamTypeIdsObject(QObject* parent = nullptr);

	QVariant GetUrlParam();
	void SetUrlParam(const QVariant& v);
	Q_INVOKABLE bool hasUrlParam();
	QVariant GetIdParam();
	void SetIdParam(const QVariant& v);
	Q_INVOKABLE bool hasIdParam();
	QVariant GetTextParam();
	void SetTextParam(const QVariant& v);
	Q_INVOKABLE bool hasTextParam();
	QVariant GetTextView();
	void SetTextView(const QVariant& v);
	Q_INVOKABLE bool hasTextView();
	QVariant GetSelectionParam();
	void SetSelectionParam(const QVariant& v);
	Q_INVOKABLE bool hasSelectionParam();
	QVariant GetSchedulerParam();
	void SetSchedulerParam(const QVariant& v);
	Q_INVOKABLE bool hasSchedulerParam();
	QVariant GetBackupSettings();
	void SetBackupSettings(const QVariant& v);
	Q_INVOKABLE bool hasBackupSettings();
	QVariant GetDatabaseAccessSettings();
	void SetDatabaseAccessSettings(const QVariant& v);
	Q_INVOKABLE bool hasDatabaseAccessSettings();
	QVariant GetParamsSet();
	void SetParamsSet(const QVariant& v);
	Q_INVOKABLE bool hasParamsSet();
	QVariant GetFileNameParam();
	void SetFileNameParam(const QVariant& v);
	Q_INVOKABLE bool hasFileNameParam();
	QVariant GetIntegerParam();
	void SetIntegerParam(const QVariant& v);
	Q_INVOKABLE bool hasIntegerParam();
	QVariant GetDoubleParam();
	void SetDoubleParam(const QVariant& v);
	Q_INVOKABLE bool hasDoubleParam();
	QVariant GetPasswordParam();
	void SetPasswordParam(const QVariant& v);
	Q_INVOKABLE bool hasPasswordParam();
	QVariant GetEnableableParam();
	void SetEnableableParam(const QVariant& v);
	Q_INVOKABLE bool hasEnableableParam();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
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

protected:
};





class CParamTypeIdsObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0, sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0, sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject> BaseClass;

	explicit CParamTypeIdsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CUrlParamObjectList;

class CUrlParamObject: public ::imtbase::CItemModelBase, public CUrlParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_scheme READ GetScheme WRITE SetScheme NOTIFY schemeChanged)
	Q_PROPERTY(QVariant m_host READ GetHost WRITE SetHost NOTIFY hostChanged)
	Q_PROPERTY(QVariant m_port READ GetPort WRITE SetPort NOTIFY portChanged)
	Q_PROPERTY(QVariant m_path READ GetPath WRITE SetPath NOTIFY pathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CUrlParamObject(QObject* parent = nullptr);

	QVariant GetScheme();
	void SetScheme(const QVariant& v);
	Q_INVOKABLE bool hasScheme();
	QVariant GetHost();
	void SetHost(const QVariant& v);
	Q_INVOKABLE bool hasHost();
	QVariant GetPort();
	void SetPort(const QVariant& v);
	Q_INVOKABLE bool hasPort();
	QVariant GetPath();
	void SetPath(const QVariant& v);
	Q_INVOKABLE bool hasPath();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void schemeChanged();
	void hostChanged();
	void portChanged();
	void pathChanged();

protected:
};





class CUrlParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0, sdl::imtbase::ImtBaseTypes::CUrlParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0, sdl::imtbase::ImtBaseTypes::CUrlParamObject> BaseClass;

	explicit CUrlParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CUrlParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CUrlParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CUrlParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CIdParamObjectList;

class CIdParamObject: public ::imtbase::CItemModelBase, public CIdParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CIdParamObject(QObject* parent = nullptr);

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





class CIdParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CIdParam::V1_0, sdl::imtbase::ImtBaseTypes::CIdParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CIdParam::V1_0, sdl::imtbase::ImtBaseTypes::CIdParamObject> BaseClass;

	explicit CIdParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CIdParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CIdParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CIdParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CIdParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CIdParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CTextParamObjectList;

class CTextParamObject: public ::imtbase::CItemModelBase, public CTextParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_text READ GetText WRITE SetText NOTIFY textChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTextParamObject(QObject* parent = nullptr);

	QVariant GetText();
	void SetText(const QVariant& v);
	Q_INVOKABLE bool hasText();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void textChanged();

protected:
};





class CTextParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0, sdl::imtbase::ImtBaseTypes::CTextParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0, sdl::imtbase::ImtBaseTypes::CTextParamObject> BaseClass;

	explicit CTextParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CTextParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CTextParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CTextParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CTextParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CTextParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CEnableableParamObjectList;

class CEnableableParamObject: public ::imtbase::CItemModelBase, public CEnableableParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CEnableableParamObject(QObject* parent = nullptr);

	QVariant GetValue();
	void SetValue(const QVariant& v);
	Q_INVOKABLE bool hasValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void valueChanged();

protected:
};





class CEnableableParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0, sdl::imtbase::ImtBaseTypes::CEnableableParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0, sdl::imtbase::ImtBaseTypes::CEnableableParamObject> BaseClass;

	explicit CEnableableParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CIntegerParamObjectList;

class CIntegerParamObject: public ::imtbase::CItemModelBase, public CIntegerParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CIntegerParamObject(QObject* parent = nullptr);

	QVariant GetValue();
	void SetValue(const QVariant& v);
	Q_INVOKABLE bool hasValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void valueChanged();

protected:
};





class CIntegerParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0, sdl::imtbase::ImtBaseTypes::CIntegerParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0, sdl::imtbase::ImtBaseTypes::CIntegerParamObject> BaseClass;

	explicit CIntegerParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CDoubleParamObjectList;

class CDoubleParamObject: public ::imtbase::CItemModelBase, public CDoubleParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDoubleParamObject(QObject* parent = nullptr);

	QVariant GetValue();
	void SetValue(const QVariant& v);
	Q_INVOKABLE bool hasValue();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void valueChanged();

protected:
};





class CDoubleParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0, sdl::imtbase::ImtBaseTypes::CDoubleParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CDoubleParam::V1_0, sdl::imtbase::ImtBaseTypes::CDoubleParamObject> BaseClass;

	explicit CDoubleParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class COptionObjectList;

class COptionObject: public ::imtbase::CItemModelBase, public COption
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QVariant m_enabled READ GetEnabled WRITE SetEnabled NOTIFY enabledChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COptionObject(QObject* parent = nullptr);

	QVariant GetId();
	void SetId(const QVariant& v);
	Q_INVOKABLE bool hasId();
	QVariant GetName();
	void SetName(const QVariant& v);
	Q_INVOKABLE bool hasName();
	QVariant GetDescription();
	void SetDescription(const QVariant& v);
	Q_INVOKABLE bool hasDescription();
	QVariant GetEnabled();
	void SetEnabled(const QVariant& v);
	Q_INVOKABLE bool hasEnabled();
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
	void enabledChanged();

protected:
};





class COptionObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::COption::V1_0, sdl::imtbase::ImtBaseTypes::COptionObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::COption::V1_0, sdl::imtbase::ImtBaseTypes::COptionObject> BaseClass;

	explicit COptionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::COptionObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::COptionObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::COptionObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::COptionObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::COptionObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class COptionsListObjectList;

class COptionsListObject: public ::imtbase::CItemModelBase, public COptionsList
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_totalCount READ GetTotalCount WRITE SetTotalCount NOTIFY totalCountChanged)
	Q_PROPERTY(QVariant m_offset READ GetOffset WRITE SetOffset NOTIFY offsetChanged)
	Q_PROPERTY(QVariant m_options READ GetOptions WRITE SetOptions NOTIFY optionsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COptionsListObject(QObject* parent = nullptr);

	QVariant GetTotalCount();
	void SetTotalCount(const QVariant& v);
	Q_INVOKABLE bool hasTotalCount();
	QVariant GetOffset();
	void SetOffset(const QVariant& v);
	Q_INVOKABLE bool hasOffset();
	QVariant GetOptions();
	void SetOptions(const QVariant& v);
	Q_INVOKABLE bool hasOptions();
	Q_INVOKABLE void emplaceOptions();
	void ResetOptions();
	Q_INVOKABLE QVariant createOptionsArrayElement(const QVariant& v);
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void totalCountChanged();
	void offsetChanged();
	void optionsChanged();

protected:
	QVariant m_optionsQObjectPtr;
};





class COptionsListObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::COptionsList::V1_0, sdl::imtbase::ImtBaseTypes::COptionsListObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::COptionsList::V1_0, sdl::imtbase::ImtBaseTypes::COptionsListObject> BaseClass;

	explicit COptionsListObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::COptionsListObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::COptionsListObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::COptionsListObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::COptionsListObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::COptionsListObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSelectionParamObjectList;

class CSelectionParamObject: public ::imtbase::CItemModelBase, public CSelectionParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_selectedIndex READ GetSelectedIndex WRITE SetSelectedIndex NOTIFY selectedIndexChanged)
	Q_PROPERTY(QVariant m_constraints READ GetConstraints WRITE SetConstraints NOTIFY constraintsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSelectionParamObject(QObject* parent = nullptr);

	QVariant GetSelectedIndex();
	void SetSelectedIndex(const QVariant& v);
	Q_INVOKABLE bool hasSelectedIndex();
	QVariant GetConstraints();
	void SetConstraints(const QVariant& v);
	Q_INVOKABLE bool hasConstraints();
	Q_INVOKABLE void emplaceConstraints();
	void ResetConstraints();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void selectedIndexChanged();
	void constraintsChanged();

protected:
	QVariant m_constraintsQObjectPtr;
};





class CSelectionParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0, sdl::imtbase::ImtBaseTypes::CSelectionParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0, sdl::imtbase::ImtBaseTypes::CSelectionParamObject> BaseClass;

	explicit CSelectionParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CSchedulerParamObjectList;

class CSchedulerParamObject: public ::imtbase::CItemModelBase, public CSchedulerParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_startTime READ GetStartTime WRITE SetStartTime NOTIFY startTimeChanged)
	Q_PROPERTY(QVariant m_interval READ GetInterval WRITE SetInterval NOTIFY intervalChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSchedulerParamObject(QObject* parent = nullptr);

	QVariant GetStartTime();
	void SetStartTime(const QVariant& v);
	Q_INVOKABLE bool hasStartTime();
	QVariant GetInterval();
	void SetInterval(const QVariant& v);
	Q_INVOKABLE bool hasInterval();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void startTimeChanged();
	void intervalChanged();

protected:
};





class CSchedulerParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0, sdl::imtbase::ImtBaseTypes::CSchedulerParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0, sdl::imtbase::ImtBaseTypes::CSchedulerParamObject> BaseClass;

	explicit CSchedulerParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CBackupSettingsObjectList;

class CBackupSettingsObject: public ::imtbase::CItemModelBase, public CBackupSettings
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_schedulerParam READ GetSchedulerParam WRITE SetSchedulerParam NOTIFY schedulerParamChanged)
	Q_PROPERTY(QVariant m_folderPath READ GetFolderPath WRITE SetFolderPath NOTIFY folderPathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CBackupSettingsObject(QObject* parent = nullptr);

	QVariant GetSchedulerParam();
	void SetSchedulerParam(const QVariant& v);
	Q_INVOKABLE bool hasSchedulerParam();
	Q_INVOKABLE void emplaceSchedulerParam();
	void ResetSchedulerParam();
	QVariant GetFolderPath();
	void SetFolderPath(const QVariant& v);
	Q_INVOKABLE bool hasFolderPath();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void schedulerParamChanged();
	void folderPathChanged();

protected:
	QVariant m_schedulerParamQObjectPtr;
};





class CBackupSettingsObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CBackupSettings::V1_0, sdl::imtbase::ImtBaseTypes::CBackupSettingsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CBackupSettings::V1_0, sdl::imtbase::ImtBaseTypes::CBackupSettingsObject> BaseClass;

	explicit CBackupSettingsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CDatabaseAccessSettingsObjectList;

class CDatabaseAccessSettingsObject: public ::imtbase::CItemModelBase, public CDatabaseAccessSettings
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_dbName READ GetDbName WRITE SetDbName NOTIFY dbNameChanged)
	Q_PROPERTY(QVariant m_host READ GetHost WRITE SetHost NOTIFY hostChanged)
	Q_PROPERTY(QVariant m_port READ GetPort WRITE SetPort NOTIFY portChanged)
	Q_PROPERTY(QVariant m_dbPath READ GetDbPath WRITE SetDbPath NOTIFY dbPathChanged)
	Q_PROPERTY(QVariant m_username READ GetUsername WRITE SetUsername NOTIFY usernameChanged)
	Q_PROPERTY(QVariant m_password READ GetPassword WRITE SetPassword NOTIFY passwordChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDatabaseAccessSettingsObject(QObject* parent = nullptr);

	QVariant GetDbName();
	void SetDbName(const QVariant& v);
	Q_INVOKABLE bool hasDbName();
	QVariant GetHost();
	void SetHost(const QVariant& v);
	Q_INVOKABLE bool hasHost();
	QVariant GetPort();
	void SetPort(const QVariant& v);
	Q_INVOKABLE bool hasPort();
	QVariant GetDbPath();
	void SetDbPath(const QVariant& v);
	Q_INVOKABLE bool hasDbPath();
	QVariant GetUsername();
	void SetUsername(const QVariant& v);
	Q_INVOKABLE bool hasUsername();
	QVariant GetPassword();
	void SetPassword(const QVariant& v);
	Q_INVOKABLE bool hasPassword();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dbNameChanged();
	void hostChanged();
	void portChanged();
	void dbPathChanged();
	void usernameChanged();
	void passwordChanged();

protected:
};





class CDatabaseAccessSettingsObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings::V1_0, sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings::V1_0, sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject> BaseClass;

	explicit CDatabaseAccessSettingsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CFileNameParamObjectList;

class CFileNameParamObject: public ::imtbase::CItemModelBase, public CFileNameParam
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_pathType READ GetPathType WRITE SetPathType NOTIFY pathTypeChanged)
	Q_PROPERTY(QVariant m_path READ GetPath WRITE SetPath NOTIFY pathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFileNameParamObject(QObject* parent = nullptr);

	QVariant GetPathType();
	void SetPathType(const QVariant& v);
	Q_INVOKABLE bool hasPathType();
	QVariant GetPath();
	void SetPath(const QVariant& v);
	Q_INVOKABLE bool hasPath();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void pathTypeChanged();
	void pathChanged();

protected:
};





class CFileNameParamObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0, sdl::imtbase::ImtBaseTypes::CFileNameParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0, sdl::imtbase::ImtBaseTypes::CFileNameParamObject> BaseClass;

	explicit CFileNameParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CParamsSetObjectList;

class CParamsSetObject: public ::imtbase::CItemModelBase, public CParamsSet
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_paramIds READ GetParamIds WRITE SetParamIds NOTIFY paramIdsChanged)
	Q_PROPERTY(QVariant m_paramTypeIds READ GetParamTypeIds WRITE SetParamTypeIds NOTIFY paramTypeIdsChanged)
	Q_PROPERTY(QVariant m_paramNames READ GetParamNames WRITE SetParamNames NOTIFY paramNamesChanged)
	Q_PROPERTY(QVariant m_paramDescriptions READ GetParamDescriptions WRITE SetParamDescriptions NOTIFY paramDescriptionsChanged)
	Q_PROPERTY(QVariant m_parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CParamsSetObject(QObject* parent = nullptr);

	QVariant GetParamIds();
	void SetParamIds(const QVariant& v);
	Q_INVOKABLE bool hasParamIds();
	QVariant GetParamTypeIds();
	void SetParamTypeIds(const QVariant& v);
	Q_INVOKABLE bool hasParamTypeIds();
	QVariant GetParamNames();
	void SetParamNames(const QVariant& v);
	Q_INVOKABLE bool hasParamNames();
	QVariant GetParamDescriptions();
	void SetParamDescriptions(const QVariant& v);
	Q_INVOKABLE bool hasParamDescriptions();
	QVariant GetParameters();
	void SetParameters(const QVariant& v);
	Q_INVOKABLE bool hasParameters();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void paramIdsChanged();
	void paramTypeIdsChanged();
	void paramNamesChanged();
	void paramDescriptionsChanged();
	void parametersChanged();

protected:
};





class CParamsSetObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0, sdl::imtbase::ImtBaseTypes::CParamsSetObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0, sdl::imtbase::ImtBaseTypes::CParamsSetObject> BaseClass;

	explicit CParamsSetObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CParamsSetObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CParamsSetObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CParamsSetObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CMimeTypeObjectList;

class CMimeTypeObject: public ::imtbase::CItemModelBase, public CMimeType
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_type READ GetType WRITE SetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_tree READ GetTree WRITE SetTree NOTIFY treeChanged)
	Q_PROPERTY(QVariant m_subType READ GetSubType WRITE SetSubType NOTIFY subTypeChanged)
	Q_PROPERTY(QVariant m_suffix READ GetSuffix WRITE SetSuffix NOTIFY suffixChanged)
	Q_PROPERTY(QVariant m_parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CMimeTypeObject(QObject* parent = nullptr);

	QVariant GetType();
	void SetType(const QVariant& v);
	Q_INVOKABLE bool hasType();
	QVariant GetTree();
	void SetTree(const QVariant& v);
	Q_INVOKABLE bool hasTree();
	QVariant GetSubType();
	void SetSubType(const QVariant& v);
	Q_INVOKABLE bool hasSubType();
	QVariant GetSuffix();
	void SetSuffix(const QVariant& v);
	Q_INVOKABLE bool hasSuffix();
	QVariant GetParameters();
	void SetParameters(const QVariant& v);
	Q_INVOKABLE bool hasParameters();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void typeChanged();
	void treeChanged();
	void subTypeChanged();
	void suffixChanged();
	void parametersChanged();

protected:
};





class CMimeTypeObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CMimeType::V1_0, sdl::imtbase::ImtBaseTypes::CMimeTypeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ImtBaseTypes::CMimeType::V1_0, sdl::imtbase::ImtBaseTypes::CMimeTypeObject> BaseClass;

	explicit CMimeTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item);
	Q_INVOKABLE sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
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
