// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>


namespace imtrest
{


class CDelegatedServletComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedServletComp);
		I_REGISTER_INTERFACE(IRequestServlet);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "Command-ID used with this handler", true, "");
		I_ASSIGN_MULTI_0(m_slaveRequestHandlersCompPtr, "RequestHandlers", "List of slave request handlers", false);
	I_END_COMPONENT

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_MULTIREF(IRequestServlet, m_slaveRequestHandlersCompPtr);
};


} // namespace imtrest


