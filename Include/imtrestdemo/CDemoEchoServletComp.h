// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>


namespace imtrestdemo
{


/**
	Servlet \c /echo: answers with the JSON description of the request that the server has received: the method, the path,
	the query, all headers (the names are in the lower case, the repeated headers are merged), the size and SHA-256 of the body
	and the body itself, if it is short. It shows how the server has understood the request: the parts of the URL and
	of the headers that came in separate reads, the merged headers, the case of the names.
	It handles all methods in the same way.
*/
class CDemoEchoServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CDemoEchoServletComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnRequestReceived(const imtrest::CHttpRequest& request) const override;
};


} // namespace imtrestdemo


