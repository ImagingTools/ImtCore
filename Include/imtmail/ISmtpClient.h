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

	/**
		Hand the message over for sending. The message is queued and sent in the background, so the call does not wait
		for the server. The result is false if the message cannot be sent at all, for example it has no recipient.
		Whether it was delivered is reported in the log of the client. A message without a sender is sent from the
		user of the server.
	*/
	virtual bool SendEmail(const ISmtpMessage& message) const = 0;
};


} // namespace imtmail

