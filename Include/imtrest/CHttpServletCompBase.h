#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ibase/IApplicationInfo.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imthttp/CHttpRequest.h>
#include <imthttp/IResponse.h>


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
		I_ASSIGN(m_commandIdCompPtr, "CommandIdParam", "Component provided list of command-ID used with corresponded handlers", false, "CommandIdParam");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", false, "ApplicationInfo");
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual imthttp::ConstResponsePtr ProcessRequest(const imthttp::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

protected:
	typedef QMap<QByteArray, QByteArray> HeadersMap;

	virtual imthttp::ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnDelete(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnPatch(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnPut(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnHead(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnOptions(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnUnknown(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr OnInvalid(
				const QByteArray& commandId,
				const imthttp::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
			const imthttp::CHttpRequest& request) const;

protected:
	virtual imthttp::ConstResponsePtr OnRequestReceived(const imthttp::CHttpRequest& request) const;
	virtual imthttp::ConstResponsePtr CreateDefaultErrorResponse(const QByteArray& errorString, const imthttp::CHttpRequest& request) const;
	static void ExtractRequestInfo(
				const imthttp::CHttpRequest& request,
				QByteArray& commandId,
				imthttp::IRequest::CommandParams& commandParams,
				HeadersMap& headers);

protected:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_REF(iprm::IIdParam, m_commandIdCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtrest


