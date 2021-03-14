#include <imtauth/CAccountInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
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
	:m_accountOwnerPtr(nullptr)
{
}


// reimplemented (IAccountInfo)

IAccountInfo::AccountType CAccountInfo::GetAccountType() const
{
	return m_accountType;
}


QString CAccountInfo::GetAccountName() const
{
	return m_accountName;
}


QString CAccountInfo::GetAccountDescription() const
{
	return m_accountDescription;
}


const IPersonInfo* CAccountInfo::GetAccountOwner() const
{
	return m_accountOwnerPtr;
}


const iauth::IRightsProvider& CAccountInfo::GetAccountRights() const
{
	return *this;
}


const iimg::IBitmap& CAccountInfo::GetAccountPicture() const
{
	return m_accountPicture;
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

	static iser::CArchiveTag accountName("AccountName", "Account name", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(accountName);
	retVal = retVal && archive.Process(m_accountName);
	retVal = retVal && archive.EndTag(accountName);

	static iser::CArchiveTag accountDescriptionTag("AccountDescription", "Account description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accountDescriptionTag);
	retVal = retVal && archive.Process(m_accountDescription);
	retVal = retVal && archive.EndTag(accountDescriptionTag);

	static iser::CArchiveTag accountPictureTag("AccountPicture", "Account picture", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(accountPictureTag);
	retVal = retVal && m_accountPicture.Serialize(archive);
	retVal = retVal && archive.EndTag(accountPictureTag);

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


} // namespace imtauth


