#include <imtbase/CObjectRepresentationControllerCompBase.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

QByteArray CObjectRepresentationControllerCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


bool CObjectRepresentationControllerCompBase::CheckPermissions(imtauth::IUserInfo::FeatureIds userPermissions, const QByteArray& paramId) const
{
	if (m_commandPermissionsProviderCompPtr.IsValid()){
		QByteArrayList elementPermissions = m_commandPermissionsProviderCompPtr->GetCommandPermissions(paramId);

		if (m_checkPermissionCompPtr.IsValid()){
			bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
			return result;
		}
	}

	return true;
}


} // namespace imtbase


