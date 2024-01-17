#pragma once

// Qt includes
#include <QtCore/QList>


namespace imtsdl
{
class CSdlField;
class CSdlType;
class CSdlMethod;

typedef QList<CSdlType> SdlTypeList;
typedef QList<CSdlField> SdlFieldList;
typedef QList<CSdlMethod> SdlMethodList;

enum KeyId
{
	KI_QUERY,
	KI_MUTATION,
	KI_SUBSCRIPTION,
};


} // namespace imtsdl
