#include <imtgql/CCommandPermissionsProviderJoinerComp.h>


namespace imtgql
{


// reimplemented (imtgql::ICommandPermissionsProvider)

CCommandPermissionsProviderJoinerComp::LogicalCheckMode CCommandPermissionsProviderJoinerComp::GetCommandPermissionCheckMode() const
{
	return CCommandPermissionsProviderJoinerComp::LogicalCheckMode::LCM_AND;
}


QByteArrayList CCommandPermissionsProviderJoinerComp::GetCommandIds() const
{
	return m_permissionsCache.keys();
}


QByteArrayList CCommandPermissionsProviderJoinerComp::GetCommandPermissions(const QByteArray& commandId) const
{
	QByteArrayList retVal;
	if(m_permissionsCache.contains(commandId)){
		retVal << m_permissionsCache.find(commandId).value();
	}
	return retVal;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CCommandPermissionsProviderJoinerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(m_commandPermissionsProviderCompPtr.IsValid()){
		UpdatePermissionsCache();
	}
}


// private methods

void CCommandPermissionsProviderJoinerComp::UpdatePermissionsCache()
{
	if(m_commandPermissionsProviderCompPtr.IsValid()){
		for(int providerIndex = 0; providerIndex < m_commandPermissionsProviderCompPtr.GetCount(); providerIndex++){
			const ICommandPermissionsProvider* currentProviderPtr = m_commandPermissionsProviderCompPtr[providerIndex];
			if(currentProviderPtr != nullptr){
				const QByteArrayList currentProviderCommandIds = currentProviderPtr->GetCommandIds();
				for(const QByteArray &commandId: currentProviderCommandIds){
					const QByteArrayList permissionsId = currentProviderPtr->GetCommandPermissions(commandId);
					if (!m_permissionsCache.contains(commandId)){
						m_permissionsCache.insert(commandId, permissionsId);
					}
				}
			}
		}
	}
}


} // namespace imtgql


