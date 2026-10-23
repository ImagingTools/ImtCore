// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>


namespace imtrestdemo
{


/**
	Servlet \c /info: gives the settings of the demo server as JSON (the limits and the timeouts, the token of the upload),
	so a client can adapt to the server it is connected to.
*/
class CDemoInfoServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CDemoInfoServletComp);
		I_ASSIGN(m_infoJsonAttrPtr, "InfoJson", "JSON text that the servlet gives", true, "{}");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

private:
	I_ATTR(QByteArray, m_infoJsonAttrPtr);
};


} // namespace imtrestdemo


