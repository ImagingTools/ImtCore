#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtmail
{


class ISmtpMailSendRequest: public istd::IChangeable
{
public:
	enum RequestState
	{
		RS_CREATED				= 0x0000, ///< Just created, and not ready to be proceed
		RS_READY_TO_PROCESSING	= 0x0001, ///< ready to proceed (When all properties has been set)
		RS_PROCESSING			= 0x0002, ///< Processing of the request started
		RS_FINISHED				= 0x0004, ///< Processing of the request has been finished
		RS_NO_ERROR				= 0x0008, ///< Processing of the request has been finished with normal result
		RS_ERROR				= 0x0010  ///< Processing of the request has been finished with error
	};

	virtual void SendMail() const = 0;
	virtual int GetState() const = 0;

	virtual const QString& GetUser() const  = 0;
	virtual void SetUser(const QString& user)  = 0;
	virtual const QString& GetPassword() const  = 0;
	virtual void SetPassword(const QString& pass)  = 0;
	virtual const QString& GetHost() const  = 0;
	virtual void SetHost(const QString& host)  = 0;
	virtual quint16 GetPort() const  = 0;
	virtual void SetMailPort(quint16 port)  = 0;
	virtual int GetTimeout() const  = 0;
	virtual void SetTimeout(int timeout)  = 0;
	virtual const QString& GetSubject() const  = 0;
	virtual void SetSubject(const QString& newSubject)  = 0;
	virtual const QString& GetBody() const  = 0;
	virtual void SetBody(const QString& newBody)  = 0;
	virtual const QStringList& GetFiles() const  = 0;
	virtual void SetFiles(const QStringList& newFiles)  = 0;
	virtual const QString& GetDestination() const  = 0;
	virtual void SetDestination(const QString& newDestination)  = 0;
	virtual const QString& GetSenderAddress() const  = 0;
	virtual void SetSenderAddress(const QString& newDestination)  = 0;	
	virtual bool IsIgnoreAllSslErrorsEnabled()const = 0;
	virtual void SetIgnoreAllSslErrors(bool enabled) = 0;
	virtual void SetBinaryData(const QString& name, const QByteArray& data) = 0;
	virtual void ClearBinaryData(const QString& name = QString()) = 0;
};


} // namespace chronosmail


