// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtdesk/CSupportTicketComp.h>
#include <imtdesk/CLabelComp.h>
#include <imtdesk/CTicketActionComp.h>
#include <imtdesk/CTicketDocumentNameProviderComp.h>
#include <imtdesk/CEntityTypeProviderComp.h>
#include <imtdesk/CDelegatedEntityTypeProviderComp.h>


/**
	ImtDeskPck package
*/
namespace ImtDeskPck
{


typedef icomp::TModelCompWrap<imtdesk::CSupportTicketComp> SupportTicket;
typedef icomp::TModelCompWrap<imtdesk::CLabelComp> Label;
typedef icomp::TModelCompWrap<imtdesk::CTicketActionComp> TicketAction;
typedef imtdesk::CTicketDocumentNameProviderComp TicketDocumentNameProvider;
typedef imtdesk::CEntityTypeProviderComp EntityTypeProvider;
typedef imtdesk::CDelegatedEntityTypeProviderComp DelegatedEntityTypeProvider;


} // namespace ImtDeskPck

