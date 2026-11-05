// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtaccount/CContactInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtaccount
{


// public static methods

QByteArray CContactInfo::GetTypeId()
{
	return QByteArrayLiteral("ContactInfo");
}


// public methods

CContactInfo::CContactInfo()
	:m_modelUpdateBridge(this, imod::CModelUpdateBridge::UF_SOURCE)
{
	m_addresses.AttachObserver(&m_modelUpdateBridge);
}


// reimplemented (IContactInfo)

QString CContactInfo::GetEmail() const
{
	return m_email;
}


void CContactInfo::SetEmail(const QString& email)
{
	if (m_email != email){
		istd::CChangeNotifier changeNotifier(this);

		m_email = email;
	}
}


QString CContactInfo::GetPhone() const
{
	return m_phone;
}


void CContactInfo::SetPhone(const QString& phone)
{
	if (m_phone != phone){
		istd::CChangeNotifier changeNotifier(this);

		m_phone = phone;
	}
}


QString CContactInfo::GetWebPage() const
{
	return m_webPage;
}


void CContactInfo::SetWebPage(const QString& webPage)
{
	if (m_webPage != webPage){
		istd::CChangeNotifier changeNotifier(this);

		m_webPage = webPage;
	}
}


IAddressProvider* CContactInfo::GetAddresses() const
{
	return const_cast<imod::TModelWrap<CAddressCollection>*>(&m_addresses);
}


// reimplemented (iser::ISerializable)

bool CContactInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag emailTag(QByteArrayLiteral("Email"), QByteArrayLiteral("Email"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(emailTag);
	retVal = retVal && archive.Process(m_email);
	retVal = retVal && archive.EndTag(emailTag);

	iser::CArchiveTag phoneTag(QByteArrayLiteral("Phone"), QByteArrayLiteral("Phone"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(phoneTag);
	retVal = retVal && archive.Process(m_phone);
	retVal = retVal && archive.EndTag(phoneTag);

	iser::CArchiveTag webPageTag(QByteArrayLiteral("WebPage"), QByteArrayLiteral("Web page"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(webPageTag);
	retVal = retVal && archive.Process(m_webPage);
	retVal = retVal && archive.EndTag(webPageTag);

	// iser::CArchiveTag addressesTag(QByteArrayLiteral("Addresses"), QByteArrayLiteral("Addresses"), iser::CArchiveTag::TT_GROUP);
	// retVal = retVal && archive.BeginTag(addressesTag);
	// retVal = retVal && m_addresses.Serialize(archive);
	// retVal = retVal && archive.EndTag(addressesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CContactInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CContactInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CContactInfo* sourcePtr = dynamic_cast<const CContactInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_email = sourcePtr->m_email;
	m_phone = sourcePtr->m_phone;
	m_webPage = sourcePtr->m_webPage;

	return m_addresses.CopyFrom(sourcePtr->m_addresses);
}


bool CContactInfo::IsEqual(const IChangeable& object) const
{
	const CContactInfo* sourcePtr = dynamic_cast<const CContactInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	bool retVal = m_email == sourcePtr->m_email;
	retVal = retVal && m_phone == sourcePtr->m_phone;
	retVal = retVal && m_webPage == sourcePtr->m_webPage;

	return retVal;
}


istd::IChangeableUniquePtr CContactInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CContactInfo);
	if (!clonePtr->CopyFrom(*this, mode)){
		return nullptr;
	}

	return clonePtr;
}


bool CContactInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_email.clear();
	m_phone.clear();
	m_webPage.clear();

	return m_addresses.ResetData(mode);
}


} // namespace imtaccount

