// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CDelegatedEntityTypeProviderComp.h>


namespace imtdesk
{


// reimplemented (imtdesk::IEntityTypeProvider)

QString CDelegatedEntityTypeProviderComp::GetEntityTypeId() const
{
	if (m_slaveEntityTypeProviderCompPtr.IsValid()) {
		return m_slaveEntityTypeProviderCompPtr->GetEntityTypeId();
	}

	return QString();
}


QString CDelegatedEntityTypeProviderComp::GetEntityTypeName() const
{
	if (m_slaveEntityTypeProviderCompPtr.IsValid()) {
		return m_slaveEntityTypeProviderCompPtr->GetEntityTypeName();
	}

	return QString();
}


} // namespace imtdesk
