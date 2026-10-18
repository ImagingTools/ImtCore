// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CExternalIdentityManagerComp.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IExternalIdentityManager)

IExternalIdentitySharedPtr CExternalIdentityManagerComp::FindIdentity(const QByteArray& providerId, const QString& subject) const
{
	if (providerId.isEmpty() || subject.isEmpty()){
		return IExternalIdentitySharedPtr();
	}

	Identities identities = FindIdentities(QByteArrayLiteral("ProviderId"), providerId, QByteArrayLiteral("Subject"), subject);
	for (const IExternalIdentitySharedPtr& identityPtr : std::as_const(identities)){
		if (identityPtr->GetProviderId() == providerId && identityPtr->GetSubject() == subject){
			return identityPtr;
		}
	}

	return IExternalIdentitySharedPtr();
}


IExternalIdentityManager::Identities CExternalIdentityManagerComp::GetUserIdentities(const QByteArray& userId) const
{
	if (userId.isEmpty()){
		return Identities();
	}

	Identities retVal;

	const Identities identities = FindIdentities(QByteArrayLiteral("UserId"), userId);
	for (const IExternalIdentitySharedPtr& identityPtr : identities){
		if (identityPtr->GetUserId() == userId){
			retVal.append(identityPtr);
		}
	}

	return retVal;
}


QByteArray CExternalIdentityManagerComp::LinkIdentity(
			const QByteArray& userId,
			const QByteArray& providerId,
			const QString& subject,
			const QString& email) const
{
	if (userId.isEmpty() || providerId.isEmpty() || subject.isEmpty()){
		return QByteArray();
	}

	if (FindIdentity(providerId, subject).IsValid()){
		SendWarningMessage(0, QStringLiteral("The '%1' account is already linked to a user").arg(QString::fromUtf8(providerId)));

		return QByteArray();
	}

	IExternalIdentityUniquePtr identityPtr = m_identityFactoryCompPtr.CreateInstance();
	if (!identityPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Unable to create an external identity link"));

		return QByteArray();
	}

	const QByteArray identityId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	const QDateTime now = QDateTime::currentDateTimeUtc();

	identityPtr->SetId(identityId);
	identityPtr->SetUserId(userId);
	identityPtr->SetProviderId(providerId);
	identityPtr->SetSubject(subject);
	identityPtr->SetEmail(email);
	identityPtr->SetLinkedAt(now);
	identityPtr->SetLastSignInAt(now);

	// The unique index on (ProviderId, Subject) rejects a concurrent second link of the same account.
	QByteArray retVal = m_identityCollectionCompPtr->InsertNewObject("ExternalIdentity", QString::fromUtf8(providerId), email, identityPtr.GetPtr(), identityId);
	if (retVal.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Unable to store the link of the '%1' account").arg(QString::fromUtf8(providerId)));
	}

	return retVal;
}


bool CExternalIdentityManagerComp::RegisterSignIn(const QByteArray& identityId, const QString& email) const
{
	IExternalIdentitySharedPtr identityPtr = GetIdentity(identityId);
	if (!identityPtr.IsValid()){
		return false;
	}

	identityPtr->SetLastSignInAt(QDateTime::currentDateTimeUtc());
	if (!email.isEmpty()){
		identityPtr->SetEmail(email);
	}

	return m_identityCollectionCompPtr->SetObjectData(identityId, *identityPtr);
}


bool CExternalIdentityManagerComp::RemoveIdentity(const QByteArray& identityId) const
{
	if (identityId.isEmpty()){
		return false;
	}

	return m_identityCollectionCompPtr->RemoveElements(imtbase::ICollectionInfo::Ids() << identityId);
}


// private methods

IExternalIdentityManager::Identities CExternalIdentityManagerComp::FindIdentities(
			const QByteArray& fieldId,
			const QVariant& value,
			const QByteArray& secondFieldId,
			const QVariant& secondValue) const
{
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter(fieldId, value));
	if (!secondFieldId.isEmpty()){
		complexFilter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter(secondFieldId, secondValue));
	}

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	Identities retVal;

	const imtbase::ICollectionInfo::Ids identityIds = m_identityCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	for (const QByteArray& identityId : identityIds){
		IExternalIdentitySharedPtr identityPtr = GetIdentity(identityId);
		if (identityPtr.IsValid()){
			retVal.append(identityPtr);
		}
	}

	return retVal;
}


IExternalIdentitySharedPtr CExternalIdentityManagerComp::GetIdentity(const QByteArray& identityId) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_identityCollectionCompPtr->GetObjectData(identityId, dataPtr)){
		return IExternalIdentitySharedPtr();
	}

	const IExternalIdentity* storedIdentityPtr = dynamic_cast<const IExternalIdentity*>(dataPtr.GetPtr());
	if (storedIdentityPtr == nullptr){
		return IExternalIdentitySharedPtr();
	}

	IExternalIdentityUniquePtr identityPtr = m_identityFactoryCompPtr.CreateInstance();
	if (!identityPtr.IsValid() || !identityPtr->CopyFrom(*storedIdentityPtr)){
		return IExternalIdentitySharedPtr();
	}

	return IExternalIdentitySharedPtr(identityPtr.PopInterfacePtr());
}


} // namespace imtauth


