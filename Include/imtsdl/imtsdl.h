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


} // namespace imtsdl
