// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtrest
{


/**
	Text IDs of the well-known protocols, as they are returned by IProtocolEngine::GetProtocolTypeId().
	The ID is a text, so the protocols that are not listed here (FTP, SMTP etc.) can be added without any change of
	the transport classes: they should not interpret the ID, they only pass it to the code that knows the protocols.
	The IDs are in the lower case.
*/
namespace ProtocolTypeIds
{


inline const QByteArray s_http = QByteArrayLiteral("http");
inline const QByteArray s_tcp = QByteArrayLiteral("tcp");
inline const QByteArray s_webSocket = QByteArrayLiteral("ws");


} // namespace ProtocolTypeIds


} // namespace imtrest


