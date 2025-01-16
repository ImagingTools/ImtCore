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
