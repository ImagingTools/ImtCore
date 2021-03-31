#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{
	class IResponse;
}


namespace imtrest
{


class CHttpServletCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CHttpServletCompBase)
		I_REGISTER_INTERFACE(IRequestHandler);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual const imtrest::IResponse* ProcessRequest(const imtrest::IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

protected:
	typedef QMap<QByteArray, QByteArray> HeadersMap;

	virtual const imtrest::IResponse* OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnPost(
				const QByteArray& commandId,
				const QByteArray body,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnDelete(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnPatch(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnPut(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnHead(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnOptions(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnUnknown(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual const imtrest::IResponse* OnInvalid(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const;

protected:
	virtual const imtrest::IResponse* OnGetRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnPostRequestReceived(
				const imtrest::CHttpRequest& request,
				const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnDeleteRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnPatchRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnPutRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnHeadRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnOptionsRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnUnknownRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;
	virtual const imtrest::IResponse* OnInvalidRequestReceived(const imtrest::CHttpRequest& request, const imtrest::IProtocolEngine& engine) const;


private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);

	typedef QMap<QString, IRequestHandler*> HandlersMap;

	HandlersMap m_handlersMap;
	QMutex m_handlersMapMutex;

	imtrest::IResponse* CreateDefaultErrorResponse(const QByteArray& errorString, const imtrest::CHttpRequest& request) const;

};


} // namespace imtrest


