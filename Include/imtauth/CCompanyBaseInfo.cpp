#include <imtauth/CCompanyBaseInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods


CCompanyBaseInfo::CCompanyBaseInfo():
	m_parentCompanyPtr(nullptr)
{
}


// reimplemented (ICompanyInfo)

const ICompanyInfo* CCompanyBaseInfo::GetParent() const
{
	return m_parentCompanyPtr;
}


const istd::TPointerVector<const ICompanyBaseInfo>& CCompanyBaseInfo::GetChildren() const
{
	return m_companyChildren;
}


// reimplemented (iser::ISerializable)

bool CCompanyBaseInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	if (m_parentCompanyPtr != nullptr){
		static iser::CArchiveTag parentCompanyTag("ParentCompany", "Parent of the company", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(parentCompanyTag);
		retVal = retVal && m_parentCompanyPtr->Serialize(archive);
		retVal = retVal && archive.EndTag(parentCompanyTag);
	}

	static iser::CArchiveTag subCompaniesTag("SubCompanies", "Subsidiary companies", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag companyTag("Company", "Company", iser::CArchiveTag::TT_GROUP, &subCompaniesTag);

	int companyCount = m_companyChildren.GetCount();

	retVal = retVal && archive.BeginMultiTag(subCompaniesTag, companyTag, companyCount);

	if (!archive.IsStoring()){
		int oldcompanyCount = m_companyChildren.GetCount();

		m_companyChildren.SetCount(companyCount);

		for (int i = oldcompanyCount; i < companyCount; ++i){
			m_companyChildren.SetElementAt(i, new imod::TModelWrap<CCompanyBaseInfo>());
		}
	}

	for (int i = 0; i < m_companyChildren.GetCount(); i++){
		const ICompanyBaseInfo* companyInfoPtr = m_companyChildren.GetAt(i);
		retVal = retVal && archive.BeginTag(companyTag);
		retVal = retVal && const_cast<ICompanyBaseInfo*>(companyInfoPtr)->Serialize(archive);
		retVal = retVal && archive.EndTag(companyTag);
	}

	retVal = retVal && archive.EndTag(subCompaniesTag);


	return retVal;
}


bool CCompanyBaseInfo::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CCompanyBaseInfo* sourcePtr = dynamic_cast<const CCompanyBaseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		bool retVal = BaseClass::CopyFrom(object);

		m_companyChildren.Reset();

		int count = sourcePtr->m_companyChildren.GetCount();

		m_companyChildren.SetCount(count);

		for (int i = 0; i < count; ++i){
			m_companyChildren.SetElementAt(i, new imod::TModelWrap<CCompanyBaseInfo>());
		}

		for (int i = 0; i < count; ++i){
			ICompanyBaseInfo* companyInfoPtr = const_cast<ICompanyBaseInfo*>(m_companyChildren.GetAt(i));
			bool result = companyInfoPtr->CopyFrom(*sourcePtr->m_companyChildren.GetAt(i));
			if (!result){
				return false;
			}
		}

		m_parentCompanyPtr = const_cast<ICompanyInfo*>(sourcePtr->GetParent());

		return retVal;
	}

	return false;
}


istd::IChangeable *CCompanyBaseInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CCompanyBaseInfo> clonePtr(new CCompanyBaseInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CCompanyBaseInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = BaseClass::ResetData(mode);

	m_parentCompanyPtr = nullptr;
	m_companyChildren.Reset();

	return retVal;
}


}


