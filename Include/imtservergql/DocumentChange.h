// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


/**
	Change-ID of the client edit carried by the request (\c imtbase::s_documentChangeIdHeaderId); empty when the client sent none.
	The server returns it in the \c DocumentChanged notification of that edit, see \c imtdoc::IDocumentService::SetDocumentData.
	The notification may reach other users, so only a UUID is taken.
*/
inline QByteArray GetDocumentChangeId(const imtgql::CGqlRequest& gqlRequest)
{
	const QByteArray changeId = gqlRequest.GetHeader(imtbase::s_documentChangeIdHeaderId);

	return imtbase::isValidUuid(changeId) ? changeId : QByteArray();
}


} // namespace imtservergql


