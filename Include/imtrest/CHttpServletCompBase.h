#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{
	class IResponse;
}


namespace imtrest
{


class CHttpServletCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CHttpServletCompBase)
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const imtrest::IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

protected:
	typedef QMap<QByteArray, QByteArray> HeadersMap;

	virtual ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnDelete(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnPatch(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnPut(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnHead(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnOptions(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnUnknown(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr OnInvalid(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const;

protected:
	virtual ConstResponsePtr OnRequestReceived(const imtrest::CHttpRequest& request) const;
	virtual ConstResponsePtr CreateDefaultErrorResponse(const QByteArray& errorString, const imtrest::CHttpRequest& request) const;
	static void ExtractRequestInfo(
				const imtrest::CHttpRequest& request,
				QByteArray& commandId,
				imtrest::IRequest::CommandParams& commandParams,
				HeadersMap& headers);

protected:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtrest


