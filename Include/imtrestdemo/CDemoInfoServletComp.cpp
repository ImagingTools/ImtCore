// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoInfoServletComp.h>


// ImtCore includes
#include <imtrestdemo/DemoServletTools.h>


namespace imtrestdemo
{


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CDemoInfoServletComp::OnGet(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return DemoServletTools::CreateResponse(request.GetProtocolEngine(), imtrest::IProtocolEngine::SC_OK, *m_infoJsonAttrPtr, "application/json");
}


} // namespace imtrestdemo
