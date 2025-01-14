#pragma once


// ACF includes
#include <istd/IChangeable.h>


#ifdef Q_OS_WIN
	#undef SetPort
	#undef GetUserName
#endif


namespace imtmail
{


class IMailSettings: virtual public istd::IChangeable
{
public:
	virtual QString GetHost() const = 0;
	virtual void SetHost(const QString& host) = 0;
	virtual int GetPort() const = 0;
	virtual void SetPort(int port) = 0;
	virtual QString GetUserName() const = 0;
	virtual void SetUserName(const QString& userName) = 0;
	virtual QString GetPassword() const = 0;
	virtual void SetPassword(const QString& password) = 0;
	virtual QString GetSenderAddress() const = 0;
	virtual void SetSenderAddress(const QString& address) = 0;
	virtual int GetRequestTimeout() const = 0;
	virtual void SetRequestTimeout(int msec) = 0;
	virtual bool GetIgnoreSslErrors() const = 0;
	virtual void SetIgnoreSslErrors(bool ignore) = 0;
};


} // namespace chronosmail


