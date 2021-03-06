#include <imtlic/CProductInstanceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtlic/ILicenseInfo.h>
#include <imtlic/IProductInfo.h>


namespace imtlic
{


// public methods

CProductInstanceInfo::CProductInstanceInfo()
	:m_customerCollectionPtr(nullptr),
	m_productCollectionPtr(nullptr)
{
}


// reimplemented (imtlic::IProductInstanceInfo)

const imtbase::IObjectCollection* CProductInstanceInfo::GetProductDatabase() const
{
	return m_productCollectionPtr;
}


const imtbase::IObjectCollection* CProductInstanceInfo::GetCustomerDatabase() const
{
	return m_customerCollectionPtr;
}


void CProductInstanceInfo::SetupProductInstance(
			const QByteArray& productId,
			const QByteArray& instanceId,
			const QByteArray& customerId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productId = productId;
	m_customerId = customerId;
	m_instanceId = instanceId;
}


void CProductInstanceInfo::AddLicense(const QByteArray & licenseId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.InsertReference(licenseId);
}


void CProductInstanceInfo::RemoveLicense(const QByteArray & licenseId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.RemoveReference(licenseId);
}


QByteArray CProductInstanceInfo::GetProductId() const
{
	return m_productId;
}


QByteArray CProductInstanceInfo::GetProductInstanceId() const
{
	return QByteArray();
}


QByteArray CProductInstanceInfo::GetCustomerId() const
{
	return QByteArray();
}


// reimplemented (imtlic::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CProductInstanceInfo::GetLicenseList() const
{
	return m_licenses;
}


const imtlic::ILicenseInfo* CProductInstanceInfo::GetLicenseInfo(const QByteArray& licenseId) const
{
	const imtbase::IObjectCollection* licenseCollectionPtr = m_licenses.GetSourceCollection();
	if (licenseCollectionPtr != nullptr){
		return dynamic_cast<const imtlic::ILicenseInfo*>(licenseCollectionPtr->GetObjectPtr(licenseId));
	}

	return nullptr;
}


// reimplemented (iser::ISerializable)

bool CProductInstanceInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	static iser::CArchiveTag instanceIdTag("InstanceId", "ID of the product instance", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(instanceIdTag);
	retVal = retVal && archive.Process(m_instanceId);
	retVal = retVal && archive.EndTag(instanceIdTag);

	static iser::CArchiveTag customerIdTag("CustomerId", "ID of the customer", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(customerIdTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(customerIdTag);

	static iser::CArchiveTag licensesTag("Licenses", "List of product licenses", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(licensesTag);
	retVal = retVal && m_licenses.Serialize(archive);
	retVal = retVal && archive.EndTag(licensesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CProductInstanceInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CProductInstanceInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CProductInstanceInfo* sourcePtr = dynamic_cast<const imtlic::CProductInstanceInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_licenses.CopyFrom(sourcePtr->m_licenses);
		m_productId = sourcePtr->m_productId;

		return true;
	}

	return false;
}


istd::IChangeable* CProductInstanceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CProductInstanceInfo> clonePtr(new CProductInstanceInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CProductInstanceInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.ResetData();

	m_productId.clear();

	return true;
}


} // namespace imtlic


