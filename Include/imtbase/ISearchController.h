#pragma once


// ImtCore includes
#include <imtbase/ISearchResults.h>


namespace imtbase
{


class ISearchController: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetControllerId() const = 0;
	virtual QString GetControllerName() const = 0;
	virtual const imtbase::ISearchResults* Search(const QString& text) const = 0;
};


} // namespace imtbase


