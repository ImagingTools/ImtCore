// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QList>


namespace imtsdl
{


enum KeyId
{
	KI_QUERY,
	KI_MUTATION,
	KI_SUBSCRIPTION,
};


struct SdlCustomSchemaKeys
{
	static const inline QString SchemaName = QStringLiteral("name");
	static const inline QString SchemaNamespace = QStringLiteral("namespace");
	static const inline QString VersionName = QStringLiteral("version");
	static const inline QString EnumConversionType = QStringLiteral("enumConversion");
};


struct EnumConversionTypes
{
	static const inline QString AsIs = QStringLiteral("asIs");
	static const inline QString UpperCamel = QStringLiteral("upperCamel");
	static const inline QString LowerCamel = QStringLiteral("lowerCamel");
	static const inline QString UpperSnake = QStringLiteral("upperSnake");
	static const inline QString LowerSnake = QStringLiteral("lowerSnake");
	static const inline QString UpperKebab = QStringLiteral("upperKebab");
	static const inline QString LowerKebab = QStringLiteral("lowerKebab");
};


/// this struct describes keys of params for SDL processors
struct ProcessorParamKeys
{
	/**
		\brief the path to the temporary directory where the processors MUST be work
		\details
			where the output files MUST be written (if available)
			represented as \c ifile::IFileNameParam with type \c ifile::IFileNameParam::PT_DIRECTORY
	*/
	static const inline QByteArray TempDirPath = QByteArrayLiteral("TempDirPath");
};


enum Priority
{
	P_C = 0,
	P_OS_API,
	P_QT,
	P_ACF,
	P_IMT,
	P_CUSTOM
};


enum Type
{
	T_HEADER,
	T_SOURCE
};


struct IncludeDirective
{
	Priority priority = P_C;
	QString remark;
	QString path;
	Type type = T_HEADER;

	bool operator == (const IncludeDirective& other) const
	{
		return
			priority == other.priority &&
			remark == other.remark &&
			path == other.path &&
			type == other.type;
	}
};


/**
	\brief The QtResourceModelParamIds struct defines param IDs of a QT resource's (QRC) model
*/
struct QtResourceModelParamIds
{
	// prefix properties
	/// defines an ID of the "prefix name" parameter, represented as \c iprm::INameParam
	static const inline QByteArray PrefixName = QByteArrayLiteral("Name");

	/// defines an ID of the "Objects" parameter, represented as ilst of parameters i.e. \c iprm::IParamsManager of \c iprm::IParamsSet
	static const inline QByteArray QrcObjects = QByteArrayLiteral("Objects");

	// QRC-object properties
	/// defines an ID of the "QRC object alias" parameter, represented as \c iprm::INameParam
	static const inline QByteArray QrcAlias = QByteArrayLiteral("Alias");

	/// defines an ID of the "QRC object name" parameter - is a relative file path, represented as \c iprm::INameParam
	static const inline QByteArray QrcFileName = QByteArrayLiteral("FileName");
};



} // namespace imtsdl



/// Custom specialization of std::hash for \c imtsdl::IncludeDirective
template<>
struct std::hash<imtsdl::IncludeDirective>
{
	size_t operator()(const imtsdl::IncludeDirective& s, size_t seed = 0) const
	{
		return qHashMulti(seed, s.priority, s.remark, s.path, s.type);
	}
};
