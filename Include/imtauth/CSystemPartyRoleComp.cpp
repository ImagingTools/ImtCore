// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CSystemPartyRoleComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CSystemPartyRoleComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetRoleId(*m_roleIdAttrPtr);
	// The catalog copies the role into an identifiable one
	SetObjectUuid(*m_roleIdAttrPtr);
	// The source texts: the server translates them into the language of the client
	SetName(m_nameAttrPtr.GetOriginalValue());
	SetDescription(m_descriptionAttrPtr.IsValid() ? m_descriptionAttrPtr.GetOriginalValue() : QString());
	SetPairedRoleId(m_pairedRoleIdAttrPtr.IsValid() ? *m_pairedRoleIdAttrPtr : QByteArray());
	SetApplicability((*m_isPartyRoleAttrPtr ? RA_PARTY : 0) | (*m_isLinkRoleAttrPtr ? RA_LINK : 0));
}


} // namespace imtauth


