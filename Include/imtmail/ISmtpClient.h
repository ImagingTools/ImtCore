// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtmail/ISmtpMessage.h>


namespace imtmail
{


class ISmtpClient: virtual public istd::IPolymorphic
{
public:
	virtual QString GetHost() const = 0;
	virtual int GetPort() const = 0;
	virtual bool SendEmail(const ISmtpMessage& message) const = 0;
};


} // namespace imtmail


