// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


