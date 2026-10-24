#pragma once

#include "ImtBaseTypes_fwd.h"

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
