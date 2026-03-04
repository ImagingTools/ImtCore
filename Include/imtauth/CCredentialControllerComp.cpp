// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCredentialControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauth
{


// protected methods

// reimplemented (iser::ICredentialController)

bool CCredentialControllerComp::CheckCredential(const QByteArray& login, const QByteArray& password) const
{
	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CCredentialControllerComp");
		return false;
	}

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CCredentialControllerComp");
		return false;
	}
	
	imtbase::CComplexCollectionFilter complexFilter;
	
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "Id";
	fieldFilter.filterValue = login;
	complexFilter.AddFieldFilter(fieldFilter);
	
	QByteArray passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + password);
	imtbase::IComplexCollectionFilter::FieldFilter passwordFieldFilter;
	passwordFieldFilter.fieldId = "PasswordHash";
	passwordFieldFilter.filterValue = passwordHash;
	complexFilter.AddFieldFilter(passwordFieldFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);

	return !userIds.isEmpty();
}


} // namespace imtauth


