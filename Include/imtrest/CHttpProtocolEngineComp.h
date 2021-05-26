#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
//#include <imtrest/CHttpResponder.h>
#include <imtrest/CHttpSender.h>
#include <imtrest/http_parser.h>


namespace imtrest
{


class CHttpProtocolEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public IProtocolEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpProtocolEngineComp);
		I_REGISTER_INTERFACE(IProtocolEngine)
	I_END_COMPONENT

	// reimplemented (IProtocolEngine)
	virtual QByteArray GetProtocolTypeId() const override;
	virtual const iser::IVersionInfo* GetProtocolVersion() const override;
	virtual bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override;
	virtual IRequest* CreateRequest(QObject* socketPtr, const IRequestHandler& requestHandler) const override;
	virtual IRequest* CreateRequestForSend(
				QObject* socketPtr,
				const IRequestHandler& requestHandler,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;

	virtual IResponse* CreateResponse(
				const IRequest& request,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;
//	virtual const IResponder& GetResponder(const IRequest* requestPtr = nullptr) const override;
		virtual const ISender& GetSender(const IRequest* requestPtr = nullptr) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
//	CHttpResponder m_responder;
		CHttpSender m_sender;
};


} // namespace imtrest


