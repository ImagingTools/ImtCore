// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtaccount/CCompanyInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtaccount
{


// public static methods

QByteArray CCompanyInfo::GetTypeId()
{
	return QByteArrayLiteral("CompanyInfo");
}


// public methods

CCompanyInfo::CCompanyInfo()
	:m_parentCompanyPtr(nullptr),
	m_modelUpdateBridge(this, imod::CModelUpdateBridge::UF_SOURCE)
{
	m_contactInfo.AttachObserver(&m_modelUpdateBridge);
}


// reimplemented (ICompanyInfo)

QString CCompanyInfo::GetLegalName() const
{
	return m_legalName;
}


void CCompanyInfo::SetLegalName(const QString& legalName)
{
	if (m_legalName != legalName){
		istd::CChangeNotifier changeNotifier(this);

		m_legalName = legalName;
	}
}


const ICompanyInfo* CCompanyInfo::GetParent() const
{
	return m_parentCompanyPtr;
}


const istd::TPointerVector<const ICompanyInfo>& CCompanyInfo::GetChildren() const
{
	return m_companyChildren;
}


// reimplemented (IContactInfoProvider)

const IContactInfo* CCompanyInfo::GetContactInfo() const
{
	return &m_contactInfo;
}


// reimplemented (iser::IObject)

QByteArray CCompanyInfo::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CCompanyInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag legalNameTag(QByteArrayLiteral("LegalName"), QByteArrayLiteral("Legal name"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(legalNameTag);
	retVal = retVal && archive.Process(m_legalName);
	retVal = retVal && archive.EndTag(legalNameTag);

	static iser::CArchiveTag contactInfoTag(QByteArrayLiteral("ContactInfo"), QByteArrayLiteral("Contact info"), iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(contactInfoTag);
	retVal = retVal && m_contactInfo.Serialize(archive);
	retVal = retVal && archive.EndTag(contactInfoTag);

	static iser::CArchiveTag subCompaniesTag(QByteArrayLiteral("SubCompanies"), QByteArrayLiteral("Subsidiary companies"), iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag companyTag(QByteArrayLiteral("Company"), QByteArrayLiteral("Company"), iser::CArchiveTag::TT_GROUP, &subCompaniesTag);

	int companyCount = m_companyChildren.GetCount();

	retVal = retVal && archive.BeginMultiTag(subCompaniesTag, companyTag, companyCount);

	if (!archive.IsStoring()){
		int oldCompanyCount = m_companyChildren.GetCount();

		m_companyChildren.SetCount(companyCount);

		for (int i = oldCompanyCount; i < companyCount; ++i){
			m_companyChildren.SetElementAt(i, new imod::TModelWrap<CCompanyInfo>());
		}
	}

	for (int i = 0; i < m_companyChildren.GetCount(); ++i){
		const ICompanyInfo* companyInfoPtr = m_companyChildren.GetAt(i);
		retVal = retVal && archive.BeginTag(companyTag);
		retVal = retVal && const_cast<ICompanyInfo*>(companyInfoPtr)->Serialize(archive);
		retVal = retVal && archive.EndTag(companyTag);
	}

	retVal = retVal && archive.EndTag(subCompaniesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CCompanyInfo::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CCompanyInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CCompanyInfo* sourcePtr = dynamic_cast<const CCompanyInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_legalName = sourcePtr->m_legalName;
	m_parentCompanyPtr = sourcePtr->m_parentCompanyPtr;

	bool retVal = m_contactInfo.CopyFrom(sourcePtr->m_contactInfo);

	m_companyChildren.Reset();

	int count = sourcePtr->m_companyChildren.GetCount();

	m_companyChildren.SetCount(count);

	for (int i = 0; i < count; ++i){
		m_companyChildren.SetElementAt(i, new imod::TModelWrap<CCompanyInfo>());
	}

	for (int i = 0; i < count; ++i){
		ICompanyInfo* companyInfoPtr = const_cast<ICompanyInfo*>(m_companyChildren.GetAt(i));
		retVal = retVal && companyInfoPtr->CopyFrom(*sourcePtr->m_companyChildren.GetAt(i));
	}

	return retVal;
}


bool CCompanyInfo::IsEqual(const IChangeable& object) const
{
	const CCompanyInfo* sourcePtr = dynamic_cast<const CCompanyInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	bool retVal = m_legalName == sourcePtr->m_legalName;
	retVal = retVal && m_parentCompanyPtr == sourcePtr->m_parentCompanyPtr;
	retVal = retVal && m_contactInfo.IsEqual(sourcePtr->m_contactInfo);

	int childCount = m_companyChildren.GetCount();
	retVal = retVal && childCount == sourcePtr->m_companyChildren.GetCount();
	for (int i = 0; retVal && i < childCount; ++i){
		const istd::IChangeable* childPtr = dynamic_cast<const istd::IChangeable*>(m_companyChildren.GetAt(i));
		const istd::IChangeable* sourceChildPtr = dynamic_cast<const istd::IChangeable*>(sourcePtr->m_companyChildren.GetAt(i));

		retVal = retVal && childPtr != nullptr && sourceChildPtr != nullptr;
		if (retVal){
			retVal = retVal && childPtr->IsEqual(*sourceChildPtr);
		}
	}

	return retVal;
}


istd::IChangeableUniquePtr CCompanyInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CCompanyInfo);
	if (!clonePtr->CopyFrom(*this, mode)){
		return nullptr;
	}

	return clonePtr;
}


bool CCompanyInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_legalName.clear();
	m_parentCompanyPtr = nullptr;
	m_companyChildren.Reset();

	return m_contactInfo.ResetData();
}


} // namespace imtaccount

