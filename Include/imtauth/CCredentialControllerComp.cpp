#include <imtauth/CCredentialControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>


namespace imtauth
{


// protected methods

// reimplemented (iser::ICredentialController)

bool CCredentialControllerComp::CheckCredential(const QByteArray& login, const QByteArray& password) const
{
	if (!m_hashCalculatorCompPtr.IsValid()){
		return false;
	}

	if (!m_userCollectionCompPtr.IsValid()){
		return false;
	}

	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam userId;
	userId.SetText(login);
	paramsSet.SetEditableParameter("Id", &userId);

	QByteArray passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + password);

	iprm::CTextParam passwordHashParam;
	passwordHashParam.SetText(passwordHash);
	paramsSet.SetEditableParameter("PasswordHash", &passwordHashParam);

	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	return userIds.size() > 0;
}


} // namespace imtauth


