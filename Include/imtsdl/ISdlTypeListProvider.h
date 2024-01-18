#pragma once


// imtsdl includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


class ISdlTypeListProvider: virtual public istd::IPolymorphic
{

public:
	virtual QStringList GetTypeNames() const = 0;
	virtual SdlTypeList GetSdlTypes() const = 0;
	virtual SdlFieldList GetFields(const QString typeName) const = 0;

};


} // namespace imtsdl


