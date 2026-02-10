// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


/**
	Publisher that uses another request handler to build the response to the client.
*/
class CGqlQueryBasedPublisherCompBase: public CGqlPublisherCompBase
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlQueryBasedPublisherCompBase);
		I_ASSIGN(m_requestHandlerCompPtr, "GqlRequestHandler", "GraphGL-request handler to create the subscription message for the subscriber", false, "GqlRequestHandler");
		I_ASSIGN(m_requestHandlerCommandIdAtrPtr, "RequestHandlerCommandId", "ID of the command used by registered handler to create the subscription message for the subscriber", true, "");
	I_END_COMPONENT;

protected:
	virtual bool Publish(bool useAwsStyle = true);

protected:
	I_REF(imtgql::IGqlRequestHandler, m_requestHandlerCompPtr);
	I_ATTR(QByteArray, m_requestHandlerCommandIdAtrPtr);
};


} // namespace imtservergql


