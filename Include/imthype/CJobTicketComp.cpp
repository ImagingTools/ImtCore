// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobTicketComp.h>


namespace imthype
{


// reimplemented (imthype::IJobTicket)

iprm::IParamsSetSharedPtr CJobTicketComp::CreateParams() const
{
	if (m_jobParamsFactPtr.IsValid()){
		return m_jobParamsFactPtr.CreateInstance();
	}

	return iprm::IParamsSetSharedPtr();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CJobTicketComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CJobTicketComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imthype


