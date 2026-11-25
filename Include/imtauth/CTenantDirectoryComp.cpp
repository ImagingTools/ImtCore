// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantDirectoryComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/CollectionSelection.h>


namespace imtauth
{


namespace
{


static const int s_maxTextLength = 2000;


} // anonymous namespace


// reimplemented (imtauth::ITenantDirectory)

ITenantDirectoryProfileUniquePtr CTenantDirectoryComp::GetProfile(const QByteArray& tenantId) const
{
	bool isStored = false;

	return ReadProfile(tenantId, isStored);
}


bool CTenantDirectoryComp::SetProfile(const ITenantDirectoryProfile& profile, QString& errorMessage)
{
	const QByteArray tenantId = profile.GetTenantId();
	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (tenantId.isEmpty() || !tenantPtr.IsValid()){
		errorMessage = QStringLiteral("A profile belongs to an organization");

		return false;
	}

	if (profile.GetDescription().size() > s_maxTextLength || profile.GetIndustry().size() > 200
				|| profile.GetCountry().size() > 100 || profile.GetWebsite().size() > 500){
		errorMessage = QStringLiteral("A text of the profile is too long");

		return false;
	}

	ITenantDirectoryProfileUniquePtr storedProfilePtr = m_profileFactoryCompPtr.CreateInstance();
	if (!storedProfilePtr.IsValid() || !storedProfilePtr->CopyFrom(profile)){
		errorMessage = QStringLiteral("The profile could not be created");

		return false;
	}

	storedProfilePtr->SetTenantName(tenantPtr->GetTenantName());
	storedProfilePtr->SetTenantActive(tenantPtr->IsActive());
	storedProfilePtr->SetIndustry(profile.GetIndustry().trimmed());
	storedProfilePtr->SetCountry(profile.GetCountry().trimmed());
	storedProfilePtr->SetWebsite(profile.GetWebsite().trimmed());
	storedProfilePtr->SetUpdatedAt(QDateTime::currentDateTimeUtc());

	bool isStored = false;
	if (!ReadProfile(tenantId, isStored).IsValid()){
		errorMessage = QStringLiteral("The profile could not be read");

		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	const bool isWritten = isStored
				? m_profileCollectionCompPtr->SetObjectData(tenantId, *storedProfilePtr)
				: !m_profileCollectionCompPtr->InsertNewObject("TenantDirectoryProfile", storedProfilePtr->GetTenantName(), QString(), storedProfilePtr.GetPtr(), tenantId).isEmpty();
	if (!isWritten){
		errorMessage = QStringLiteral("The profile could not be stored");

		return false;
	}

	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CTenantDirectoryComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_tenantCollectionModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_tenantCollectionModelCompPtr.GetPtr(), 0);
	}
}


void CTenantDirectoryComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CTenantDirectoryComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	const QByteArray tenantId = changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray();
	if (!tenantId.isEmpty()){
		UpdateTenantInfo(tenantId);
	}
}


// private methods

void CTenantDirectoryComp::UpdateTenantInfo(const QByteArray& tenantId)
{
	bool isStored = false;
	ITenantDirectoryProfileUniquePtr profilePtr = ReadProfile(tenantId, isStored);
	if (!profilePtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("The directory profile of the organization '%1' could not be read: the search may show its former name and state")
					.arg(QString::fromUtf8(tenantId)), "CTenantDirectoryComp");

		return;
	}

	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (!isStored || !tenantPtr.IsValid()){
		return;
	}

	if (profilePtr->GetTenantName() == tenantPtr->GetTenantName() && profilePtr->IsTenantActive() == tenantPtr->IsActive()){
		return;
	}

	profilePtr->SetTenantName(tenantPtr->GetTenantName());
	profilePtr->SetTenantActive(tenantPtr->IsActive());

	istd::CChangeNotifier changeNotifier(this);

	if (!m_profileCollectionCompPtr->SetObjectData(tenantId, *profilePtr)){
		SendErrorMessage(0, QStringLiteral("The directory profile of the organization '%1' keeps its former name and state: the search shows them")
					.arg(QString::fromUtf8(tenantId)), "CTenantDirectoryComp");
	}
}


ITenantDirectoryProfileUniquePtr CTenantDirectoryComp::ReadProfile(const QByteArray& tenantId, bool& isStored) const
{
	isStored = false;
	if (tenantId.isEmpty()){
		return nullptr;
	}

	ITenantDirectoryProfileUniquePtr retVal = m_profileFactoryCompPtr.CreateInstance();
	if (!retVal.IsValid()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_profileCollectionCompPtr->GetObjectData(tenantId, dataPtr)){
		const ITenantDirectoryProfile* storedProfilePtr = dynamic_cast<const ITenantDirectoryProfile*>(dataPtr.GetPtr());
		if (storedProfilePtr == nullptr || storedProfilePtr->GetTenantId() != tenantId || !retVal->CopyFrom(*storedProfilePtr)){
			return nullptr;
		}

		isStored = true;

		return retVal;
	}

	// No profile, or one that could not be read
	bool isRead = true;
	const QByteArrayList ids = CollectionSelection::SelectIds(*m_profileCollectionCompPtr, CollectionSelection::ColumnEquals("TenantId", tenantId), QByteArrayList(), &isRead);
	if (!isRead || ids.contains(tenantId)){
		return nullptr;
	}

	retVal->SetTenantId(tenantId);

	return retVal;
}


} // namespace imtauth


