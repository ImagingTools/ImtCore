#include <imtbase/CObjectRepresentationControllerCompBase.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtbase
{


// protected methods

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


// reimplemented (IRepresentationController)

QByteArray CObjectRepresentationControllerCompBase::GetModelId() const
{
	return *m_objectIdAttrPtr;
}


bool CObjectRepresentationControllerCompBase::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	if (m_objectIdAttrPtr.IsValid()){
		representation.SetData("Id", *m_objectIdAttrPtr);
	}

	QByteArray languageId;
	if (m_translationManagerCompPtr.IsValid()){
		const iprm::IOptionsList& optionsList = m_translationManagerCompPtr->GetLanguagesInfo();
		int languageIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();

		languageId = optionsList.GetOptionId(languageIndex);
	}

	QString paramName;
	if (m_objectNameAttrPtr.IsValid()){
		paramName = *m_objectNameAttrPtr;
	}

	if (m_translationManagerCompPtr.IsValid()){
		QString elementNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), paramName.toUtf8(), languageId, QByteArray("Attribute"));

		paramName = elementNameTr;
	}

	representation.SetData("Name", paramName);

	if (m_qmlPathAttrPtr.IsValid()){
		representation.SetData("Source", *m_qmlPathAttrPtr);
	}

	return GetRepresentationFromValue(dataModel, representation, paramsPtr);
}


} // namespace imtbase


