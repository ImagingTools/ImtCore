#include <imtserverapp/CSingleCommandPermissionsProviderComp.h>


namespace imtserverapp
{


// reimplemented (imtgql::ICommandPermissionsProvider)

CSingleCommandPermissionsProviderComp::LogicalCheckMode CSingleCommandPermissionsProviderComp::GetCommandPermissionCheckMode() const
{
	return LCM_AND;
}


QByteArrayList CSingleCommandPermissionsProviderComp::GetCommandIds() const
{
	QByteArrayList retVal;

	retVal.push_back(*m_commandIdAttrPtr);

	return retVal;
}


QByteArrayList CSingleCommandPermissionsProviderComp::GetCommandPermissions(const QByteArray& /*commandId*/) const
{
	QByteArrayList retVal;

	if (m_commandPermissionsAttrPtr.IsValid()){
		for (int i = 0; i < m_commandPermissionsAttrPtr.GetCount(); i++){
			retVal << m_commandPermissionsAttrPtr[i];
		}
	}

	return retVal;
}


} // namespace imtserverapp


