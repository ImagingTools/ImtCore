// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CCommandGroupPermissionsProviderComp.h>


namespace imtserverapp
{


// public methods

// reimplemented (ICommandPermissionsProvider)

ICommandPermissionsProvider::LogicalCheckMode CCommandGroupPermissionsProviderComp::GetCommandPermissionCheckMode() const
{
	return LCM_AND;
}


QByteArrayList CCommandGroupPermissionsProviderComp::GetCommandIds() const
{
	QByteArrayList retVal;
	if (!m_commandPermissionsAttrPtr.IsValid() || m_commandPermissionsAttrPtr.GetCount() == 0){
		return retVal;
	}

	const QByteArray collectionId = m_collectionIdAttrPtr.IsValid() ? *m_collectionIdAttrPtr : QByteArray();
	for (int index = 0; index < m_commandIdsAttrPtr.GetCount(); index++){
		retVal.append(collectionId.isEmpty() ? m_commandIdsAttrPtr[index] : collectionId + '/' + m_commandIdsAttrPtr[index]);
	}

	return retVal;
}


QByteArrayList CCommandGroupPermissionsProviderComp::GetCommandPermissions(const QByteArray& commandId) const
{
	QByteArrayList retVal;
	if (!GetCommandIds().contains(commandId)){
		return retVal;
	}

	for (int index = 0; index < m_commandPermissionsAttrPtr.GetCount(); index++){
		retVal.append(m_commandPermissionsAttrPtr[index]);
	}

	return retVal;
}


} // namespace imtserverapp
