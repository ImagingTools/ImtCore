// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>


namespace imtrestdemo
{


/**
	Servlet \c /dynamic/<name>: shows where the dynamic policy of the body (imtrest::CHttpDynamicBodyPolicyComp) has received the body of
	the request. The policy chooses it by the size of the body, so the servlet answers with the JSON:
	- \c storage: \c memory if the body is in the memory of the request, \c file if it is written to a file by parts;
	- \c size and \c sha256 of the body that the servlet has read from that place (the file is read by parts, the body is not loaded into the memory);
	- \c fileName of the temporary file for \c storage \c file. The file is deleted after the answer: the servlet does not take it, so a body
	  that nobody needs does not stay on the disk.
	A body that is too big is refused by the policy with \c 413 before the body is sent, the servlet is not called.
	It handles all methods in the same way.
*/
class CDemoDynamicBodyServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CDemoDynamicBodyServletComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnRequestReceived(const imtrest::CHttpRequest& request) const override;
};


} // namespace imtrestdemo


