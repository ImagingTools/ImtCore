#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlDocumentType.h>


namespace imtsdl
{


class ISdlDocumentTypeListProvider: virtual public istd::IPolymorphic
{

public:
	virtual SdlDocumentTypeList GetDocumentTypes(bool onlyLocal) const = 0;
};


} // namespace imtsdl


