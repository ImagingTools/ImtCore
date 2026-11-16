// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtNetwork/QNetworkReply>


namespace imtclientgql
{


/**
	The reason a remote GraphQL server failed a request, as its client may show it: the message of the first GraphQL error in
	the body, else the HTTP status. The address of the server stays out of it.
	\param body	The body of the reply, already read.
*/
QString GetReplyErrorMessage(const QNetworkReply& reply, const QByteArray& body);


} // namespace imtclientgql
