#pragma once

// Qt includes
#include <QtCore/QList>


namespace imtsdl
{


class CSdlField;
class CSdlType;
class CSdlRequest;
class CSdlDocumentType;

typedef QList<CSdlType> SdlTypeList;
typedef QList<CSdlField> SdlFieldList;
typedef QList<CSdlRequest> SdlRequestList;
typedef QList<CSdlDocumentType> SdlDocumentTypeList;

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
};


} // namespace imtsdl
