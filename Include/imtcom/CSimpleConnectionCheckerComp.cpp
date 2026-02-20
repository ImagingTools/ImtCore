// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CSimpleConnectionCheckerComp.h>


namespace imtcom
{


// public methods
CSimpleConnectionCheckerComp::CSimpleConnectionCheckerComp() : m_status(CS_UNKNOWN)
{
}

// reimplemented (IConnectionStatusProvider)

IConnectionStatusProvider::ConnectionStatus CSimpleConnectionCheckerComp::GetConnectionStatus() const
{
	return IConnectionStatusProvider::ConnectionStatus::CS_DISCONNECTED;
}


} // namespace imtcom


