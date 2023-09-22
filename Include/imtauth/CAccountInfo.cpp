#include <imtauth/CAccountInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


// public static methods

QByteArray CAccountInfo::GetTypeId()
{
	return "AccountInfo";
}


// public methods

CAccountInfo::CAccountInfo()
	:m_accountType(AT_COMPANY),
	m_contactUpdateBridge(this)
{
//	IAddressManager* addressManagerPtr = dynamic_cast<imtauth::IAddressManager*>(
//				const_cast<IAddressProvider*>(m_contact.GetAddresses()));
//	if (addressManagerPtr != nullptr){
//		CAddress address;
//		addressManagerPtr->AddAddress(&address);
//	}

//	m_contact.AttachObserver(&m_contactUpdateBridge);
}


CAccountInfo::~CAccountInfo()
{
	m_contactUpdateBridge.EnsureModelsDetached();
}


// reimplemented (IAccountInfo)

IAccountInfo::AccountType CAccountInfo::GetAccountType() const
{
	return m_accountType;
}


void CAccountInfo::SetAccountType(AccountType accountType)
{
	if (m_accountType != accountType){
		istd::CChangeNotifier notifier(this);

		m_accountType = accountType;
	}
}


QString CAccountInfo::GetAccountName() const
{
	return m_accountName;
}


void CAccountInfo::SetAccountName(QString accountName)
{
	if (m_accountName != accountName){
		istd::CChangeNotifier notifier(this);

		m_accountName = accountName;
	}
}


QString CAccountInfo::GetAccountDescription() const
{
	return m_accountDescription;
}


void CAccountInfo::SetAccountDescription(QString accountDescription)
{
	if (m_accountDescription != accountDescription){
		istd::CChangeNotifier notifier(this);

		m_accountDescription = accountDescription;
	}
}


const iimg::IBitmap& CAccountInfo::GetAccountPicture() const
{
	return m_accountPicture;
}


void CAccountInfo::SetAccountPicture(const iimg::IBitmap& picture)
{
	istd::CChangeNotifier notifier(this);

	m_accountPicture.CopyFrom(picture);
}


const IContactInfo* CAccountInfo::GetAccountOwner() const
{
	return nullptr;
}


void CAccountInfo::SetAccountOwner(const IContactInfo& /*ownerInfo*/)
{
	istd::CChangeNotifier changeNorifier(this);

//	m_contact.CopyFrom(ownerInfo);
}


const iauth::IRightsProvider& CAccountInfo::GetAccountRights() const
{
	return *this;
}


// reimplemented (iauth::IRightsProvider)

bool CAccountInfo::HasRight(const QByteArray& /*operationId*/, bool /*beQuiet*/) const
{
	return false;
}


// reimplemented (iser::IObject)

QByteArray CAccountInfo::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CAccountInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	int accountType = m_accountType;

	iser::CArchiveTag accountTypeTag("AccountType", "Account type", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(accountTypeTag);
	retVal = retVal && archive.Process(accountType);
	retVal = retVal && archive.EndTag(accountTypeTag);

	if (!archive.IsStoring()){
		m_accountType = AT_PERSON;

		if (accountType >= AT_PERSON && accountType <= AT_COMPANY){
			m_accountType = (AccountType)accountType;
		}
	}

	iser::CArchiveTag accountNameTag("AccountName", "Account name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(accountNameTag);
	retVal = retVal && archive.Process(m_accountName);
	retVal = retVal && archive.EndTag(accountNameTag);

	iser::CArchiveTag accountDescriptionTag("AccountDescription", "Account description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accountDescriptionTag);
	retVal = retVal && archive.Process(m_accountDescription);
	retVal = retVal && archive.EndTag(accountDescriptionTag);

	iser::CArchiveTag accountPictureTag("AccountPicture", "Account picture", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(accountPictureTag);
	retVal = retVal && m_accountPicture.Serialize(archive);
	retVal = retVal && archive.EndTag(accountPictureTag);

//	iser::CArchiveTag contactTag("Contact", "Contact", iser::CArchiveTag::TT_GROUP);
//	retVal = retVal && archive.BeginTag(contactTag);
//	retVal = retVal && m_contact.Serialize(archive);
//	retVal = retVal && archive.EndTag(contactTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAccountInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CAccountInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);


	const CAccountInfo* sourcePtr = dynamic_cast<const CAccountInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_accountType = sourcePtr->m_accountType;
		m_accountName = sourcePtr->m_accountName;
		m_accountDescription = sourcePtr->m_accountDescription;

		bool retVal = true;

		retVal = retVal && m_accountPicture.CopyFrom(sourcePtr->GetAccountPicture());
//		retVal = retVal && m_contact.CopyFrom(sourcePtr->m_contact);

		return retVal;
	}

	return false;
}


istd::IChangeable* CAccountInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CAccountInfo> clonePtr(new CAccountInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CAccountInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_accountType = AT_PERSON;
	m_accountName.clear();
	m_accountDescription.clear();
	m_accountPicture.ResetImage();

	return true;
}


} // namespace imtauth


