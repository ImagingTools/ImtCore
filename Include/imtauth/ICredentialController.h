// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


class ICredentialController: virtual public istd::IPolymorphic
{
public:
	virtual bool CheckCredential(const QByteArray& login, const QByteArray& password) const = 0;
};


} // namespace imtauth


