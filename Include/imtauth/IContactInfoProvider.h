// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtauth
{


class IContactInfo;


/**
	Provider of persons.
	\ingroup Authentification
*/
class IPersonInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetPersonList() const = 0;
	virtual const IContactInfo* GetPerson(const QByteArray& personId) const = 0;
};


} // namespace imtauth


