// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CTicketDocumentNameProviderComp.h>


// ImtCore includes
#include <imtdesk/ISupportTicket.h>


namespace imtdesk
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CTicketDocumentNameProviderComp::GetDefaultDocumentName(const QByteArray& /*objectId*/, const istd::IChangeable& document) const
{
	const ISupportTicket* ticketPtr = dynamic_cast<const ISupportTicket*>(&document);
	if (ticketPtr != nullptr){
		return ticketPtr->GetTitle();
	}

	return QString();
}


} // namespace imtdesk


