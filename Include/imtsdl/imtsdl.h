#pragma once

// Qt includes
#include <QtCore/QList>


namespace imtsdl
{


class CSdlField;
class CSdlType;
class CSdlRequest;
class CSdlDocumentType;
class CSdlEnum;
class CSdlUnion;

typedef QList<CSdlType> SdlTypeList;
typedef QList<CSdlField> SdlFieldList;
typedef QList<CSdlRequest> SdlRequestList;
typedef QList<CSdlDocumentType> SdlDocumentTypeList;
typedef QList<CSdlEnum> SdlEnumList;
typedef QList<CSdlUnion> SdlUnionList;

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
};


} // namespace imtsdl
