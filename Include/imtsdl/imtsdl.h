#pragma once

// Qt includes
#include <QtCore/QList>


namespace imtsdl
{


class CSdlField;
class CSdlType;
class CSdlMethod;
class CSdlRequest;

typedef QList<CSdlType> SdlTypeList;
typedef QList<CSdlField> SdlFieldList;
typedef QList<CSdlMethod> SdlMethodList;
typedef QList<CSdlRequest> SdlRequestList;

enum KeyId
{
	KI_QUERY,
	KI_MUTATION,
	KI_SUBSCRIPTION,
};


} // namespace imtsdl
