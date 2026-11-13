// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IHttpRequestBodyPolicy.h>


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
		I_ASSIGN(m_bodyPolicyCompPtr, "BodyPolicy", "Policy that decides about the body of a request after its headers are received, for example refuses a request with 413 or 401 before the body is read. Without it all bodies are received with the limit MaxBodySize", false, "BodyPolicy");
		I_ASSIGN(m_maxBodySizeAttrPtr, "MaxBodySize", "Maximal size of the body of a request in bytes if the policy does not set its own limit. The request with a bigger body is refused with 413 before the body is read. 0 - no limit", true, 0);
	I_END_COMPONENT

	// reimplemented (IProtocolEngine)
	virtual QByteArray GetProtocolTypeId() const override;
	virtual const iser::IVersionInfo* GetProtocolVersion() const override;
	virtual bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override;
	virtual IRequestUniquePtr CreateRequest(const IRequestServlet& requestHandler) const override;
	virtual IRequestUniquePtr CreateRequestForSend(
				const IRequestServlet& requestHandler,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;
	virtual IResponseUniquePtr CreateResponse(
				const IRequest& request,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;

private:
	I_REF(IHttpRequestBodyPolicy, m_bodyPolicyCompPtr);
	I_ATTR(int, m_maxBodySizeAttrPtr);
};


} // namespace imtrest


