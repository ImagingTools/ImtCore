// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		I_ASSIGN(m_commandIdCompPtr, "CommandIdParam", "Component provided list of command-ID used with corresponded handlers", false, "CommandIdParam");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", false, "ApplicationInfo");
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

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
	I_REF(iprm::IIdParam, m_commandIdCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtrest


