#include <imtlic/CProductLicensingInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtlic
{


// public methods

// reimplemented (imtlic::IProductInfo)

const IProductInfo* CProductLicensingInfo::GetProductFamilyInfo() const
{
	return nullptr;
}


QByteArray CProductLicensingInfo::GetProductId() const
{
	return m_productId;
}


void CProductLicensingInfo::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = productId;
	}
}


// reimplemented (imtlic::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CProductLicensingInfo::GetLicenseList() const
{
	return m_licenses;
}


const imtlic::ILicenseInfo* CProductLicensingInfo::GetLicenseInfo(const QByteArray& licenseId) const
{
	return m_licenses.GetLicenseInfo(licenseId);
}


// reimplemented (iprm::INameParam)

const QString& CProductLicensingInfo::GetName() const
{
	return m_productName;
}


void CProductLicensingInfo::SetName(const QString& name)
{
	if (m_productName != name){
		istd::CChangeNotifier changeNotifier(this);

		m_productName = name;
	}
}


bool CProductLicensingInfo::IsNameFixed() const
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CProductLicensingInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	static iser::CArchiveTag productNameTag("ProductName", "Name of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productNameTag);
	retVal = retVal && archive.Process(m_productName);
	retVal = retVal && archive.EndTag(productNameTag);

	static iser::CArchiveTag licensesTag("Licenses", "List of product licenses", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(licensesTag);
	retVal = retVal && m_licenses.Serialize(archive);
	retVal = retVal && archive.EndTag(licensesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CProductLicensingInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CProductLicensingInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CProductLicensingInfo* sourcePtr = dynamic_cast<const imtlic::CProductLicensingInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_licenses.CopyFrom(sourcePtr->m_licenses);
		m_productId = sourcePtr->m_productId;
		m_productName = sourcePtr->m_productName;

		return true;
	}

	return false;
}


istd::IChangeable* CProductLicensingInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CProductLicensingInfo> clonePtr(new CProductLicensingInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CProductLicensingInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.ResetData();

	m_productId.clear();

	m_productName.clear();

	return true;
}


} // namespace imtlic


