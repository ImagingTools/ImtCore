// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CObjectRepresentationControllerCompBase.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>
#include <iqt/iqt.h>

// Qt includes
#include <QtCore/QJsonValue>

// ImtCore includes
#include <imtserverapp/imtserverapp.h>


namespace imtserverapp
{


// protected methods

bool CObjectRepresentationControllerCompBase::CheckPermissions(
			const imtauth::IUserInfo::FeatureIds& userPermissions,
			const QByteArray& paramId,
			const iprm::IParamsSet* paramsPtr) const
{
	return IsElementAccessible(
			m_commandPermissionsProviderCompPtr.GetPtr(),
			m_checkPermissionCompPtr.GetPtr(),
			paramId,
			userPermissions,
			false,
			GetPermissionPath(paramsPtr));
}


// reimplemented (IRepresentationController)

QByteArray CObjectRepresentationControllerCompBase::GetModelId() const
{
	return *m_objectIdAttrPtr;
}


bool CObjectRepresentationControllerCompBase::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, QJsonObject& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		SendErrorMessage(0, QString("Unable to get representation model from data model. Model is not supported."));

		return false;
	}

	if (m_objectIdAttrPtr.IsValid()){
		representation.insert(QStringLiteral("Id"), QString::fromUtf8(*m_objectIdAttrPtr));
	}

	QByteArray languageId;

	if (paramsPtr != nullptr){
		iprm::TParamsPtr<iprm::IIdParam> languageParamPtr(paramsPtr, "LanguageParam");
		if (languageParamPtr.IsValid()){
			languageId = languageParamPtr->GetId();
		}
	}

	if (m_translationManagerCompPtr.IsValid() && languageId.isEmpty()){
		const iprm::IOptionsList& optionsList = m_translationManagerCompPtr->GetLanguagesInfo();
		int languageIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
		if (languageIndex >= 0){
			languageId = optionsList.GetOptionId(languageIndex);
		}
	}

	QString paramName;
	if (m_objectNameAttrPtr.IsValid()){
		paramName = *m_objectNameAttrPtr;
	}

	if (m_translationManagerCompPtr.IsValid()){
		QString elementNameTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), paramName.toUtf8(), languageId, QByteArray("Attribute"));

		paramName = elementNameTr;
	}

	representation.insert(QStringLiteral("Name"), paramName);

	if (m_qmlPathAttrPtr.IsValid()){
		representation.insert(QStringLiteral("Source"), QString::fromUtf8(*m_qmlPathAttrPtr));
	}

	return GetRepresentationFromValue(dataModel, representation, paramsPtr);
}


} // namespace imtserverapp
