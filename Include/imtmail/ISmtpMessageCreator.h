// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtmail/ISmtpMessage.h>


namespace imtmail
{


class ISmtpMessageCreator: virtual public istd::IPolymorphic
{
public:
	virtual ISmtpMessage* CreateMessage() const = 0;
};


} // namespace imtmail
