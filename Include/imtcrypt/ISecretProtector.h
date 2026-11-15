// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcrypt
{


/**
	Protection of a secret kept at rest, e.g. a token stored in a file.
*/
class ISecretProtector: virtual public istd::IPolymorphic
{
public:
	virtual bool Protect(const QByteArray& secret, QByteArray& protectedSecret) const = 0;
	virtual bool Unprotect(const QByteArray& protectedSecret, QByteArray& secret) const = 0;
};


} // namespace imtcrypt


