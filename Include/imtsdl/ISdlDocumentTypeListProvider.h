#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlDocumentType.h>


namespace imtsdl
{


class ISdlDocumentTypeListProvider: virtual public istd::IPolymorphic
{

public:
	virtual SdlDocumentTypeList GetDocumentTypes() const = 0;
};


} // namespace imtsdl


