// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>


namespace imtrest
{


class CHttpRedirectComp:
		public ilog::CLoggerComponentBase,
		virtual public IRequestServlet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpRedirectComp)
		I_REGISTER_INTERFACE(IRequestServlet)
		I_ASSIGN(m_redirectToCommandIdAttrPtr, "RedirectToCommandId", "Command-ID used with corresponded handlers", true, "");
	I_END_COMPONENT

	// reimplemented (icomp::IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

private:
	I_ATTR(QByteArray, m_redirectToCommandIdAttrPtr);
};


} // namespace imtrest


