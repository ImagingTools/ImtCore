#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}

namespace imtstyle
{


class IDesignSchemaInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetDesignSchemaList() const = 0;
};


} // namespace imtstyle


