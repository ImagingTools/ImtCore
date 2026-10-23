// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>
#include <imtrestdemo/IDemoSettingsController.h>


namespace imtrestdemo
{


/**
	Servlet \c /admin/settings: the settings of the running server.
	- \c GET: the settings as JSON (the same names as the options of the command line, see CDemoServer::GetSettingsJson());
	- \c PUT: the JSON object with the settings to change. If something is changed, the server is restarted in a moment with the
	  new settings, the answer of the request tells what will be used (the new port and token). If the new settings cannot
	  be started (for example, the port is busy), the server goes back to the previous ones, the reason is shown by \c lastError of
	  the \c GET. A wrong value gives 400 and nothing is changed.
	The access needs the token (see the policy of the path \c /admin in CDemoServer).
*/
class CDemoAdminServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CDemoAdminServletComp);
		I_ASSIGN(m_settingsControllerCompPtr, "SettingsController", "Gives and changes the settings of the server", true, "SettingsController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual imtrest::ConstResponsePtr OnPut(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

private:
	I_REF(IDemoSettingsController, m_settingsControllerCompPtr);
};


} // namespace imtrestdemo


