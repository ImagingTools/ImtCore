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
	:m_contactCollectionPtr(nullptr),
	m_accountType(AT_PERSON)
{
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


QString CAccountInfo::GetAccountOwnerEMail() const
{
	return m_contactEMail;
}


IAccountInfo::ContactInfoPtr CAccountInfo::GetAccountOwner() const
{
	ContactInfoPtr contactInfoPtr;

	if (!m_contactEMail.isEmpty()){
		if (m_contactCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids ids = m_contactCollectionPtr->GetElementIds();
			for (const QByteArray& id : ids){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_contactCollectionPtr->GetObjectData(id, dataPtr)){
					IContactInfo* contactPtr = dynamic_cast<IContactInfo*>(dataPtr.GetPtr());
					if (contactPtr != nullptr){
						if (contactPtr->GetEMail() == m_contactEMail){
							contactInfoPtr.SetPtr(new CContactInfo);
							contactInfoPtr->CopyFrom(*contactPtr);
						}
					}
				}
			}
		}
	}

	return contactInfoPtr;
}


void CAccountInfo::SetAccountOwner(const QString& contactEMail)
{
	if (m_contactEMail != contactEMail){
		istd::CChangeNotifier notifier(this);

		m_contactEMail = contactEMail;
	}
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

	static iser::CArchiveTag accountTypeTag("AccountType", "Account type", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(accountTypeTag);
	retVal = retVal && archive.Process(accountType);
	retVal = retVal && archive.EndTag(accountTypeTag);

	if (!archive.IsStoring()){
		m_accountType = AT_PERSON;

		if (accountType >= AT_PERSON && accountType <= AT_COMPANY){
			m_accountType = (AccountType)accountType;
		}
	}

	static iser::CArchiveTag accountNameTag("AccountName", "Account name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(accountNameTag);
	retVal = retVal && archive.Process(m_accountName);
	retVal = retVal && archive.EndTag(accountNameTag);

	static iser::CArchiveTag accountDescriptionTag("AccountDescription", "Account description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accountDescriptionTag);
	retVal = retVal && archive.Process(m_accountDescription);
	retVal = retVal && archive.EndTag(accountDescriptionTag);

	static iser::CArchiveTag accountPictureTag("AccountPicture", "Account picture", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(accountPictureTag);
	retVal = retVal && m_accountPicture.Serialize(archive);
	retVal = retVal && archive.EndTag(accountPictureTag);

	static iser::CArchiveTag contactEMailTag("ContactEMail", "Contact EMail", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contactEMailTag);
	retVal = retVal && archive.Process(m_contactEMail);
	retVal = retVal && archive.EndTag(contactEMailTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAccountInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CAccountInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IAccountInfo* sourcePtr = dynamic_cast<const IAccountInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_accountName = sourcePtr->GetAccountName();
		m_accountDescription = sourcePtr->GetAccountDescription();
		m_accountPicture.CopyFrom(sourcePtr->GetAccountPicture());

		return true;
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

	m_accountName.clear();
	m_accountDescription.clear();
	m_accountPicture.ResetImage();

	return true;
}


// protected methods

void CAccountInfo::OnContactCollectionUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* productCollectionPtr)
{
}


} // namespace imtauth


