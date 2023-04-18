#include <imtauth/CCompanyInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (IAccountBaseInfo)

QString CCompanyInfo::GetAccountName() const
{
	return m_accountName;
}


QString CCompanyInfo::GetAccountDescription() const
{
	return m_accountDescription;
}


CAddress CCompanyInfo::GetAddress() const
{
	return m_address;
}


QString CCompanyInfo::GetMail() const
{
	return m_mail;
}


// reimplemented (ICompanyInfo)

QString CCompanyInfo::GetCompanyName() const
{
	return m_companyName;
}


void CCompanyInfo::SetAccountName(const QString &accountName)
{
	if (m_accountName != accountName){
		istd::CChangeNotifier changeNotifier(this);

		m_accountName = accountName;
	}
}


void CCompanyInfo::SetAccountDescription(const QString &accountDescription)
{
	if (m_accountDescription != accountDescription){
		istd::CChangeNotifier changeNotifier(this);

		m_accountDescription = accountDescription;
	}
}


void CCompanyInfo::SetAddress(const CAddress &address)
{
	if (!m_address.IsEqual(address)){
		istd::CChangeNotifier changeNotifier(this);

		m_address = address;
	}
}


void CCompanyInfo::SetMail(const QString &mail)
{
	if (m_mail != mail){
		istd::CChangeNotifier changeNotifier(this);

		m_mail = mail;
	}
}


QByteArrayList CCompanyInfo::GetGroups() const
{
	return m_groupIds;
}


bool CCompanyInfo::AddGroup(const QByteArray& groupId)
{
	if (!m_groupIds.contains(groupId)){
		istd::CChangeNotifier changeNotifier(this);

		m_groupIds << groupId;

		return true;
	}

	return false;
}


bool CCompanyInfo::RemoveGroup(const QByteArray& groupId)
{
	bool result = m_groupIds.removeOne(groupId);
	if (result){
		istd::CChangeNotifier changeNotifier(this);
	}

	return result;
}


void CCompanyInfo::SetCompanyName(const QString &companyName)
{
	if (m_companyName != companyName){
		istd::CChangeNotifier changeNotifier(this);

		m_companyName = companyName;
	}
}


// reimplemented (iser::ISerializable)

bool CCompanyInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag accountNameTag("AccountName", "Account name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(accountNameTag);
	retVal = retVal && archive.Process(m_accountName);
	retVal = retVal && archive.EndTag(accountNameTag);

	static iser::CArchiveTag accountDescriptionTag("Description", "Account description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accountDescriptionTag);
	retVal = retVal && archive.Process(m_accountDescription);
	retVal = retVal && archive.EndTag(accountDescriptionTag);

	static iser::CArchiveTag addressTag("Address", "Address", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(addressTag);
	retVal = retVal && m_address.Serialize(archive);
	retVal = retVal && archive.EndTag(addressTag);

	static iser::CArchiveTag mailTag("Mail", "Mail", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mailTag);
	retVal = retVal && archive.Process(m_mail);
	retVal = retVal && archive.EndTag(mailTag);

	static iser::CArchiveTag companyNameTag("CompanyName", "Company name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(companyNameTag);
	retVal = retVal && archive.Process(m_companyName);
	retVal = retVal && archive.EndTag(companyNameTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_groupIds, "Groups", "Group");

	return retVal;
}


bool CCompanyInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CCompanyInfo* sourcePtr = dynamic_cast<const CCompanyInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_accountName = sourcePtr->m_accountName;
		m_accountDescription = sourcePtr->m_accountDescription;
		m_address = sourcePtr->m_address;
		m_mail = sourcePtr->m_mail;
		m_companyName = sourcePtr->m_companyName;
		m_groupIds = sourcePtr->m_groupIds;

		return true;
	}

	return false;
}


istd::IChangeable *CCompanyInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CCompanyInfo> clonePtr(new CCompanyInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CCompanyInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_accountName.clear();
	m_accountDescription.clear();
	m_mail.clear();
	m_companyName.clear();
	m_groupIds.clear();

	return true;
}


}


