#pragma once

#include "ImtBaseTypes_fwd.h"

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
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtsdl/TListModelBase.h>
#include <imtsdl/TElementList.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::V1_0::imtbase
{


inline namespace ImtBaseTypesSdlEnums
{



Q_NAMESPACE


} // inline namespace ImtBaseTypesSdlEnums


class CTimeRange
{
public:
	struct TimeRangeFields
	{
		static const inline QString Begin = "Begin";
		static const inline QString End = "End";
	};

	istd::TNullableValue<QString> Begin;
	istd::TNullableValue<QString> End;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CTimeRange& other) const;
	[[nodiscard]] bool operator!=(const CTimeRange& other) const {return !(operator==(other));}
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

class CSdlSize
{
public:
	struct SdlSizeFields
	{
		static const inline QString Width = "width";
		static const inline QString Height = "height";
	};

	istd::TNullableValue<double> width;
	istd::TNullableValue<double> height;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSdlSize& other) const;
	[[nodiscard]] bool operator!=(const CSdlSize& other) const {return !(operator==(other));}
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

class CSdlPoint
{
public:
	struct SdlPointFields
	{
		static const inline QString X = "x";
		static const inline QString Y = "y";
	};

	istd::TNullableValue<double> x;
	istd::TNullableValue<double> y;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSdlPoint& other) const;
	[[nodiscard]] bool operator!=(const CSdlPoint& other) const {return !(operator==(other));}
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

class CParamTypeIds
{
public:
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

	istd::TNullableValue<QByteArray> UrlParam;
	istd::TNullableValue<QByteArray> IdParam;
	istd::TNullableValue<QByteArray> TextParam;
	istd::TNullableValue<QByteArray> TextView;
	istd::TNullableValue<QByteArray> SelectionParam;
	istd::TNullableValue<QByteArray> SchedulerParam;
	istd::TNullableValue<QByteArray> BackupSettings;
	istd::TNullableValue<QByteArray> DatabaseAccessSettings;
	istd::TNullableValue<QByteArray> ParamsSet;
	istd::TNullableValue<QByteArray> FileNameParam;
	istd::TNullableValue<QByteArray> IntegerParam;
	istd::TNullableValue<QByteArray> DoubleParam;
	istd::TNullableValue<QByteArray> PasswordParam;
	istd::TNullableValue<QByteArray> EnableableParam;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CParamTypeIds& other) const;
	[[nodiscard]] bool operator!=(const CParamTypeIds& other) const {return !(operator==(other));}
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

class CUrlParam
{
public:
	struct UrlParamFields
	{
		static const inline QString Scheme = "scheme";
		static const inline QString Host = "host";
		static const inline QString Port = "port";
		static const inline QString Path = "path";
	};

	istd::TNullableValue<QString> scheme;
	istd::TNullableValue<QString> host;
	istd::TNullableValue<int> port;
	istd::TNullableValue<QString> path;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CUrlParam& other) const;
	[[nodiscard]] bool operator!=(const CUrlParam& other) const {return !(operator==(other));}
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

class CIdParam
{
public:
	struct IdParamFields
	{
		static const inline QString Id = "id";
	};

	istd::TNullableValue<QByteArray> id;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CIdParam& other) const;
	[[nodiscard]] bool operator!=(const CIdParam& other) const {return !(operator==(other));}
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

class CTextParam
{
public:
	struct TextParamFields
	{
		static const inline QString Text = "text";
	};

	istd::TNullableValue<QString> text;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CTextParam& other) const;
	[[nodiscard]] bool operator!=(const CTextParam& other) const {return !(operator==(other));}
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

class CEnableableParam
{
public:
	struct EnableableParamFields
	{
		static const inline QString Value = "value";
	};

	istd::TNullableValue<bool> value;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CEnableableParam& other) const;
	[[nodiscard]] bool operator!=(const CEnableableParam& other) const {return !(operator==(other));}
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

class CIntegerParam
{
public:
	struct IntegerParamFields
	{
		static const inline QString Value = "value";
	};

	istd::TNullableValue<int> value;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CIntegerParam& other) const;
	[[nodiscard]] bool operator!=(const CIntegerParam& other) const {return !(operator==(other));}
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

class CDoubleParam
{
public:
	struct DoubleParamFields
	{
		static const inline QString Value = "value";
	};

	istd::TNullableValue<double> value;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CDoubleParam& other) const;
	[[nodiscard]] bool operator!=(const CDoubleParam& other) const {return !(operator==(other));}
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

class COption
{
public:
	struct OptionFields
	{
		static const inline QString Id = "id";
		static const inline QString Name = "name";
		static const inline QString Description = "description";
		static const inline QString Enabled = "enabled";
	};

	istd::TNullableValue<QByteArray> id;
	istd::TNullableValue<QString> name;
	istd::TNullableValue<QString> description;
	istd::TNullableValue<bool> enabled;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const COption& other) const;
	[[nodiscard]] bool operator!=(const COption& other) const {return !(operator==(other));}
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

class COptionsList
{
public:
	struct OptionsListFields
	{
		static const inline QString TotalCount = "totalCount";
		static const inline QString Offset = "offset";
		static const inline QString Options = "options";
	};

	istd::TNullableValue<int> totalCount;
	istd::TNullableValue<int> offset;
	istd::TNullableValue<imtsdl::TElementList<COption>> options;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const COptionsList& other) const;
	[[nodiscard]] bool operator!=(const COptionsList& other) const {return !(operator==(other));}
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

class CSelectionParam
{
public:
	struct SelectionParamFields
	{
		static const inline QString SelectedIndex = "selectedIndex";
		static const inline QString Constraints = "constraints";
	};

	istd::TNullableValue<int> selectedIndex;
	istd::TNullableValue<COptionsList> constraints;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSelectionParam& other) const;
	[[nodiscard]] bool operator!=(const CSelectionParam& other) const {return !(operator==(other));}
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

class CSchedulerParam
{
public:
	struct SchedulerParamFields
	{
		static const inline QString StartTime = "startTime";
		static const inline QString Interval = "interval";
	};

	istd::TNullableValue<QString> startTime;
	istd::TNullableValue<int> interval;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CSchedulerParam& other) const;
	[[nodiscard]] bool operator!=(const CSchedulerParam& other) const {return !(operator==(other));}
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

class CBackupSettings
{
public:
	struct BackupSettingsFields
	{
		static const inline QString SchedulerParam = "schedulerParam";
		static const inline QString FolderPath = "folderPath";
	};

	istd::TNullableValue<CSchedulerParam> schedulerParam;
	istd::TNullableValue<QString> folderPath;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CBackupSettings& other) const;
	[[nodiscard]] bool operator!=(const CBackupSettings& other) const {return !(operator==(other));}
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

class CDatabaseAccessSettings
{
public:
	struct DatabaseAccessSettingsFields
	{
		static const inline QString DbName = "dbName";
		static const inline QString Host = "host";
		static const inline QString Port = "port";
		static const inline QString DbPath = "dbPath";
		static const inline QString Username = "username";
		static const inline QString Password = "password";
	};

	istd::TNullableValue<QString> dbName;
	istd::TNullableValue<QString> host;
	istd::TNullableValue<int> port;
	istd::TNullableValue<QString> dbPath;
	istd::TNullableValue<QString> username;
	istd::TNullableValue<QString> password;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CDatabaseAccessSettings& other) const;
	[[nodiscard]] bool operator!=(const CDatabaseAccessSettings& other) const {return !(operator==(other));}
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

class CFileNameParam
{
public:
	struct FileNameParamFields
	{
		static const inline QString PathType = "pathType";
		static const inline QString Path = "path";
	};

	istd::TNullableValue<int> pathType;
	istd::TNullableValue<QString> path;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CFileNameParam& other) const;
	[[nodiscard]] bool operator!=(const CFileNameParam& other) const {return !(operator==(other));}
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

class CParamsSet
{
public:
	struct ParamsSetFields
	{
		static const inline QString ParamIds = "paramIds";
		static const inline QString ParamTypeIds = "paramTypeIds";
		static const inline QString ParamNames = "paramNames";
		static const inline QString ParamDescriptions = "paramDescriptions";
		static const inline QString Parameters = "parameters";
	};

	istd::TNullableValue<imtsdl::TElementList<QByteArray>> paramIds;
	istd::TNullableValue<imtsdl::TElementList<QByteArray>> paramTypeIds;
	istd::TNullableValue<imtsdl::TElementList<QString>> paramNames;
	istd::TNullableValue<imtsdl::TElementList<QString>> paramDescriptions;
	istd::TNullableValue<imtsdl::TElementList<QString>> parameters;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CParamsSet& other) const;
	[[nodiscard]] bool operator!=(const CParamsSet& other) const {return !(operator==(other));}
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

class CMimeType
{
public:
	struct MimeTypeFields
	{
		static const inline QString Type = "type";
		static const inline QString Tree = "tree";
		static const inline QString SubType = "subType";
		static const inline QString Suffix = "suffix";
		static const inline QString Parameters = "parameters";
	};

	istd::TNullableValue<QString> type;
	istd::TNullableValue<imtsdl::TElementList<QString>> tree;
	istd::TNullableValue<QString> subType;
	istd::TNullableValue<QString> suffix;
	istd::TNullableValue<imtsdl::TElementList<QString>> parameters;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CMimeType& other) const;
	[[nodiscard]] bool operator!=(const CMimeType& other) const {return !(operator==(other));}
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




class CTimeRangeObjectList;

class CTimeRangeObject: public ::imtbase::CItemModelBase, public CTimeRange
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_begin READ GetBegin WRITE SetBegin NOTIFY beginChanged)
	Q_PROPERTY(QVariant m_end READ GetEnd WRITE SetEnd NOTIFY endChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTimeRangeObject(QObject* parent = nullptr);

	QVariant GetBegin();
	QString Get__Typename() {return QStringLiteral("TimeRange");}
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





class CTimeRangeObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CTimeRange, sdl::V1_0::imtbase::CTimeRangeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CTimeRange, sdl::V1_0::imtbase::CTimeRangeObject> BaseClass;

	explicit CTimeRangeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CTimeRangeObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CTimeRangeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CTimeRangeObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CTimeRangeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CTimeRangeObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CSdlSizeObjectList;

class CSdlSizeObject: public ::imtbase::CItemModelBase, public CSdlSize
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_width READ GetWidth WRITE SetWidth NOTIFY widthChanged)
	Q_PROPERTY(QVariant m_height READ GetHeight WRITE SetHeight NOTIFY heightChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSdlSizeObject(QObject* parent = nullptr);

	QVariant GetWidth();
	QString Get__Typename() {return QStringLiteral("SdlSize");}
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





class CSdlSizeObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSdlSize, sdl::V1_0::imtbase::CSdlSizeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSdlSize, sdl::V1_0::imtbase::CSdlSizeObject> BaseClass;

	explicit CSdlSizeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CSdlSizeObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CSdlSizeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CSdlSizeObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CSdlSizeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CSdlSizeObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CSdlPointObjectList;

class CSdlPointObject: public ::imtbase::CItemModelBase, public CSdlPoint
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_x READ GetX WRITE SetX NOTIFY xChanged)
	Q_PROPERTY(QVariant m_y READ GetY WRITE SetY NOTIFY yChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSdlPointObject(QObject* parent = nullptr);

	QVariant GetX();
	QString Get__Typename() {return QStringLiteral("SdlPoint");}
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





class CSdlPointObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSdlPoint, sdl::V1_0::imtbase::CSdlPointObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSdlPoint, sdl::V1_0::imtbase::CSdlPointObject> BaseClass;

	explicit CSdlPointObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CSdlPointObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CSdlPointObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CSdlPointObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CSdlPointObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CSdlPointObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CParamTypeIdsObjectList;

class CParamTypeIdsObject: public ::imtbase::CItemModelBase, public CParamTypeIds
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
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
	QString Get__Typename() {return QStringLiteral("ParamTypeIds");}
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





class CParamTypeIdsObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CParamTypeIds, sdl::V1_0::imtbase::CParamTypeIdsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CParamTypeIds, sdl::V1_0::imtbase::CParamTypeIdsObject> BaseClass;

	explicit CParamTypeIdsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CParamTypeIdsObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CParamTypeIdsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CParamTypeIdsObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CParamTypeIdsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CParamTypeIdsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CUrlParamObjectList;

class CUrlParamObject: public ::imtbase::CItemModelBase, public CUrlParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_scheme READ GetScheme WRITE SetScheme NOTIFY schemeChanged)
	Q_PROPERTY(QVariant m_host READ GetHost WRITE SetHost NOTIFY hostChanged)
	Q_PROPERTY(QVariant m_port READ GetPort WRITE SetPort NOTIFY portChanged)
	Q_PROPERTY(QVariant m_path READ GetPath WRITE SetPath NOTIFY pathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CUrlParamObject(QObject* parent = nullptr);

	QVariant GetScheme();
	QString Get__Typename() {return QStringLiteral("UrlParam");}
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





class CUrlParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CUrlParam, sdl::V1_0::imtbase::CUrlParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CUrlParam, sdl::V1_0::imtbase::CUrlParamObject> BaseClass;

	explicit CUrlParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CUrlParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CUrlParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CUrlParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CUrlParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CUrlParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CIdParamObjectList;

class CIdParamObject: public ::imtbase::CItemModelBase, public CIdParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CIdParamObject(QObject* parent = nullptr);

	QVariant GetId();
	QString Get__Typename() {return QStringLiteral("IdParam");}
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





class CIdParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CIdParam, sdl::V1_0::imtbase::CIdParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CIdParam, sdl::V1_0::imtbase::CIdParamObject> BaseClass;

	explicit CIdParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CIdParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CIdParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CIdParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CIdParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CIdParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CTextParamObjectList;

class CTextParamObject: public ::imtbase::CItemModelBase, public CTextParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_text READ GetText WRITE SetText NOTIFY textChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTextParamObject(QObject* parent = nullptr);

	QVariant GetText();
	QString Get__Typename() {return QStringLiteral("TextParam");}
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





class CTextParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CTextParam, sdl::V1_0::imtbase::CTextParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CTextParam, sdl::V1_0::imtbase::CTextParamObject> BaseClass;

	explicit CTextParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CTextParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CTextParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CTextParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CTextParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CTextParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CEnableableParamObjectList;

class CEnableableParamObject: public ::imtbase::CItemModelBase, public CEnableableParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CEnableableParamObject(QObject* parent = nullptr);

	QVariant GetValue();
	QString Get__Typename() {return QStringLiteral("EnableableParam");}
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





class CEnableableParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CEnableableParam, sdl::V1_0::imtbase::CEnableableParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CEnableableParam, sdl::V1_0::imtbase::CEnableableParamObject> BaseClass;

	explicit CEnableableParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CEnableableParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CEnableableParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CEnableableParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CEnableableParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CEnableableParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CIntegerParamObjectList;

class CIntegerParamObject: public ::imtbase::CItemModelBase, public CIntegerParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CIntegerParamObject(QObject* parent = nullptr);

	QVariant GetValue();
	QString Get__Typename() {return QStringLiteral("IntegerParam");}
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





class CIntegerParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CIntegerParam, sdl::V1_0::imtbase::CIntegerParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CIntegerParam, sdl::V1_0::imtbase::CIntegerParamObject> BaseClass;

	explicit CIntegerParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CIntegerParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CIntegerParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CIntegerParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CIntegerParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CIntegerParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CDoubleParamObjectList;

class CDoubleParamObject: public ::imtbase::CItemModelBase, public CDoubleParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDoubleParamObject(QObject* parent = nullptr);

	QVariant GetValue();
	QString Get__Typename() {return QStringLiteral("DoubleParam");}
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





class CDoubleParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CDoubleParam, sdl::V1_0::imtbase::CDoubleParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CDoubleParam, sdl::V1_0::imtbase::CDoubleParamObject> BaseClass;

	explicit CDoubleParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CDoubleParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CDoubleParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CDoubleParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CDoubleParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CDoubleParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class COptionObjectList;

class COptionObject: public ::imtbase::CItemModelBase, public COption
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_id READ GetId WRITE SetId NOTIFY idChanged)
	Q_PROPERTY(QVariant m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(QVariant m_description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QVariant m_enabled READ GetEnabled WRITE SetEnabled NOTIFY enabledChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COptionObject(QObject* parent = nullptr);

	QVariant GetId();
	QString Get__Typename() {return QStringLiteral("Option");}
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





class COptionObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::COption, sdl::V1_0::imtbase::COptionObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::COption, sdl::V1_0::imtbase::COptionObject> BaseClass;

	explicit COptionObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::COptionObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::COptionObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::COptionObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::COptionObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::COptionObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class COptionsListObjectList;

class COptionsListObject: public ::imtbase::CItemModelBase, public COptionsList
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_totalCount READ GetTotalCount WRITE SetTotalCount NOTIFY totalCountChanged)
	Q_PROPERTY(QVariant m_offset READ GetOffset WRITE SetOffset NOTIFY offsetChanged)
	Q_PROPERTY(QVariant m_options READ GetOptions WRITE SetOptions NOTIFY optionsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	COptionsListObject(QObject* parent = nullptr);

	QVariant GetTotalCount();
	QString Get__Typename() {return QStringLiteral("OptionsList");}
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





class COptionsListObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::COptionsList, sdl::V1_0::imtbase::COptionsListObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::COptionsList, sdl::V1_0::imtbase::COptionsListObject> BaseClass;

	explicit COptionsListObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::COptionsListObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::COptionsListObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::COptionsListObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::COptionsListObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::COptionsListObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CSelectionParamObjectList;

class CSelectionParamObject: public ::imtbase::CItemModelBase, public CSelectionParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_selectedIndex READ GetSelectedIndex WRITE SetSelectedIndex NOTIFY selectedIndexChanged)
	Q_PROPERTY(QVariant m_constraints READ GetConstraints WRITE SetConstraints NOTIFY constraintsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSelectionParamObject(QObject* parent = nullptr);

	QVariant GetSelectedIndex();
	QString Get__Typename() {return QStringLiteral("SelectionParam");}
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





class CSelectionParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSelectionParam, sdl::V1_0::imtbase::CSelectionParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSelectionParam, sdl::V1_0::imtbase::CSelectionParamObject> BaseClass;

	explicit CSelectionParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CSelectionParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CSelectionParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CSelectionParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CSelectionParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CSelectionParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CSchedulerParamObjectList;

class CSchedulerParamObject: public ::imtbase::CItemModelBase, public CSchedulerParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_startTime READ GetStartTime WRITE SetStartTime NOTIFY startTimeChanged)
	Q_PROPERTY(QVariant m_interval READ GetInterval WRITE SetInterval NOTIFY intervalChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CSchedulerParamObject(QObject* parent = nullptr);

	QVariant GetStartTime();
	QString Get__Typename() {return QStringLiteral("SchedulerParam");}
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





class CSchedulerParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSchedulerParam, sdl::V1_0::imtbase::CSchedulerParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CSchedulerParam, sdl::V1_0::imtbase::CSchedulerParamObject> BaseClass;

	explicit CSchedulerParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CSchedulerParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CSchedulerParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CSchedulerParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CSchedulerParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CSchedulerParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CBackupSettingsObjectList;

class CBackupSettingsObject: public ::imtbase::CItemModelBase, public CBackupSettings
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_schedulerParam READ GetSchedulerParam WRITE SetSchedulerParam NOTIFY schedulerParamChanged)
	Q_PROPERTY(QVariant m_folderPath READ GetFolderPath WRITE SetFolderPath NOTIFY folderPathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CBackupSettingsObject(QObject* parent = nullptr);

	QVariant GetSchedulerParam();
	QString Get__Typename() {return QStringLiteral("BackupSettings");}
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





class CBackupSettingsObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CBackupSettings, sdl::V1_0::imtbase::CBackupSettingsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CBackupSettings, sdl::V1_0::imtbase::CBackupSettingsObject> BaseClass;

	explicit CBackupSettingsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CBackupSettingsObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CBackupSettingsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CBackupSettingsObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CBackupSettingsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CBackupSettingsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CDatabaseAccessSettingsObjectList;

class CDatabaseAccessSettingsObject: public ::imtbase::CItemModelBase, public CDatabaseAccessSettings
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
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
	QString Get__Typename() {return QStringLiteral("DatabaseAccessSettings");}
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





class CDatabaseAccessSettingsObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CDatabaseAccessSettings, sdl::V1_0::imtbase::CDatabaseAccessSettingsObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CDatabaseAccessSettings, sdl::V1_0::imtbase::CDatabaseAccessSettingsObject> BaseClass;

	explicit CDatabaseAccessSettingsObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CFileNameParamObjectList;

class CFileNameParamObject: public ::imtbase::CItemModelBase, public CFileNameParam
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_pathType READ GetPathType WRITE SetPathType NOTIFY pathTypeChanged)
	Q_PROPERTY(QVariant m_path READ GetPath WRITE SetPath NOTIFY pathChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFileNameParamObject(QObject* parent = nullptr);

	QVariant GetPathType();
	QString Get__Typename() {return QStringLiteral("FileNameParam");}
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





class CFileNameParamObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CFileNameParam, sdl::V1_0::imtbase::CFileNameParamObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CFileNameParam, sdl::V1_0::imtbase::CFileNameParamObject> BaseClass;

	explicit CFileNameParamObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CFileNameParamObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CFileNameParamObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CFileNameParamObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CFileNameParamObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CFileNameParamObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CParamsSetObjectList;

class CParamsSetObject: public ::imtbase::CItemModelBase, public CParamsSet
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_paramIds READ GetParamIds WRITE SetParamIds NOTIFY paramIdsChanged)
	Q_PROPERTY(QVariant m_paramTypeIds READ GetParamTypeIds WRITE SetParamTypeIds NOTIFY paramTypeIdsChanged)
	Q_PROPERTY(QVariant m_paramNames READ GetParamNames WRITE SetParamNames NOTIFY paramNamesChanged)
	Q_PROPERTY(QVariant m_paramDescriptions READ GetParamDescriptions WRITE SetParamDescriptions NOTIFY paramDescriptionsChanged)
	Q_PROPERTY(QVariant m_parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CParamsSetObject(QObject* parent = nullptr);

	QVariant GetParamIds();
	QString Get__Typename() {return QStringLiteral("ParamsSet");}
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





class CParamsSetObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CParamsSet, sdl::V1_0::imtbase::CParamsSetObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CParamsSet, sdl::V1_0::imtbase::CParamsSetObject> BaseClass;

	explicit CParamsSetObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CParamsSetObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CParamsSetObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CParamsSetObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CParamsSetObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CParamsSetObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};



class CMimeTypeObjectList;

class CMimeTypeObject: public ::imtbase::CItemModelBase, public CMimeType
{
	Q_OBJECT
	Q_PROPERTY(QString __typename READ Get__Typename CONSTANT)
	Q_PROPERTY(QVariant m_type READ GetType WRITE SetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_tree READ GetTree WRITE SetTree NOTIFY treeChanged)
	Q_PROPERTY(QVariant m_subType READ GetSubType WRITE SetSubType NOTIFY subTypeChanged)
	Q_PROPERTY(QVariant m_suffix READ GetSuffix WRITE SetSuffix NOTIFY suffixChanged)
	Q_PROPERTY(QVariant m_parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CMimeTypeObject(QObject* parent = nullptr);

	QVariant GetType();
	QString Get__Typename() {return QStringLiteral("MimeType");}
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





class CMimeTypeObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CMimeType, sdl::V1_0::imtbase::CMimeTypeObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CMimeType, sdl::V1_0::imtbase::CMimeTypeObject> BaseClass;

	explicit CMimeTypeObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const override;
	Q_INVOKABLE int getItemsCount() const override;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CMimeTypeObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CMimeTypeObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CMimeTypeObject* item);
	Q_INVOKABLE void removeElement(int index) override;
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CMimeTypeObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CMimeTypeObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) const override;

signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterImtBaseTypesQmlTypes()
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

} // namespace sdl::V1_0::imtbase


Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CTimeRange);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CSdlSize);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CSdlPoint);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CParamTypeIds);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CUrlParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CIdParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CTextParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CEnableableParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CIntegerParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CDoubleParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::COption);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::COptionsList);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CSelectionParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CSchedulerParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CBackupSettings);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CDatabaseAccessSettings);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CFileNameParam);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CParamsSet);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CMimeType);
