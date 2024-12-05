#pragma once


// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ICacheController.h>


namespace imtsdl
{


class ICacheMultiManager: virtual public istd::IPolymorphic
{

public:
	virtual bool Init(const QList<std::shared_ptr<ICacheController>>& controllers) = 0;

	virtual bool ResolveIncludePathForType(const CSdlType& type, QString& foundPath) const = 0;
};


} // namespace imtsdl


