// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserDocumentValidatorComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauth
{


bool CUserDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(&document);
	if (userInfoPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type for user validation");
		return false;
	}

	const QByteArray username = userInfoPtr->GetId();
	if (username.isEmpty()){
		errorMessage = QStringLiteral("Username can't be empty!");
		return false;
	}

	const QString name = userInfoPtr->GetName();
	if (name.isEmpty()){
		errorMessage = QStringLiteral("Name cannot be empty");
		return false;
	}

	if (!m_userCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("User collection is not configured");
		return false;
	}

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "Id";
	fieldFilter.filterValue = username;

	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	for (const QByteArray& userObjectId : userIds){
		if (userObjectId == objectId){
			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
			const imtauth::IUserInfo* currentUserInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
			if (currentUserInfoPtr != nullptr){
				if (currentUserInfoPtr->GetId().toLower() == username.toLower()){
					errorMessage = QStringLiteral("Username already exists");
					return false;
				}
			}
		}
	}

	return true;
}


} // namespace imtauth
