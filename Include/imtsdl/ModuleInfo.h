#pragma once


// ACF includes
#include<istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QList>


class QJsonObject;


/**
	Classes, that describes a SDL module info (used file with .jsmi)(JsonSdlModuleInfo) extention.
	This classes are used to store and read info of module
	\note this code is auto generated from \file ./SDL/ModuleInfo.sdl args:(-GSSDL/ModuleInfo.sdl -OSDL/Output --GG=2 --CPP -MJSON --auto-link=2 --auto-join)
*/
namespace imtsdl
{

/**
	A special variable, that can be a part of the file path.
	\warning MUST be replaced to absolute dir of file, where this variable placed.
	\note can be used in \c CEntity::TargerHeaderFilePath, to indicate, that the generated file path is related to the directory of a module file.
*/
static inline const QString s_moduleDirVariable = ("${ModuleDir}");


class CSchemaParams: public virtual istd::IPolymorphic
{

public:
	std::optional<QString> Name;
	std::optional<QString> Namespace;
	std::optional<QString> Version;

	// serialize methods
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
};


class CEntity: public virtual istd::IPolymorphic
{

public:
	std::optional<QString> Name;
	std::optional<QString> TargerHeaderFilePath;
	std::optional<QString> QmlModuleName;

	// serialize methods
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
};


class CModule: public virtual istd::IPolymorphic
{

public:
	std::optional<QList<CEntity>> Types;
	std::optional<QList<CEntity>> Enums;
	std::optional<QList<CEntity>> Unions;
	std::optional<CSchemaParams> Params;

	// serialize methods
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
};


} // namespace imtsdl






