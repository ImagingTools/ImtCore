#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtmail
{


class ISmtpMessage: virtual public iser::ISerializable
{
public:
	virtual QString GetFrom() const = 0;
	virtual void SetFrom(const QString& from) = 0;
	virtual QString GetTo() const = 0;
	virtual void SetTo(const QString& to) = 0;
	virtual QString GetSubject() const = 0;
	virtual void SetSubject(const QString& subject) = 0;
	virtual QString GetBody() const = 0;
	virtual void SetBody(const QString& body) = 0;
};


} // namespace imtmail
