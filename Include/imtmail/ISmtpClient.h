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


