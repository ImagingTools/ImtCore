// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketSender.h>
#include <imtrest/http_parser.h>


namespace imtrest
{


class CWebSocketProtocolEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public IProtocolEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketProtocolEngineComp);
		I_REGISTER_INTERFACE(IProtocolEngine)
	I_END_COMPONENT

	// reimplemented (IProtocolEngine)
	virtual QByteArray GetProtocolTypeId() const override;
	virtual const iser::IVersionInfo* GetProtocolVersion() const override;
	virtual bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override;
	virtual imtrest::IRequestUniquePtr CreateRequest(const IRequestServlet& requestHandler) const override;
	virtual imtrest::IRequestUniquePtr CreateRequestForSend(
				const IRequestServlet& requestHandler,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;
	virtual imtrest::IResponseUniquePtr CreateResponse(
				const IRequest& request,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
};


} // namespace imtrest


