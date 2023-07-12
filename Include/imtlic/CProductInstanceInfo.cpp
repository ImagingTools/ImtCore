#include <imtlic/CProductInstanceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtlic/ILicenseInstance.h>
#include <imtlic/IProductLicensingInfo.h>


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
	m_instanceId = instanceId;
	m_customerId = customerId;
}


void CProductInstanceInfo::AddLicense(const QByteArray& licenseId, const QDateTime& expirationDate)
{
	if (m_productId.isEmpty()){
		return;
	}
	LicenseInstancePtr licenseInstancePtr(new CLicenseInstance);
	licenseInstancePtr->SetLicenseId(licenseId);
	licenseInstancePtr->SetExpiration(expirationDate);

	m_licenses[licenseId] = licenseInstancePtr;

	m_licenseContainerInfo.InsertItem(licenseId, "", "");
}


void CProductInstanceInfo::RemoveLicense(const QByteArray& licenseId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.remove(licenseId);

	m_licenseContainerInfo.RemoveItem(licenseId);
}


void CProductInstanceInfo::ClearLicenses()
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.clear();
}


QByteArray CProductInstanceInfo::GetProductId() const
{
	return m_productId;
}


QByteArray CProductInstanceInfo::GetProductInstanceId() const
{
	return m_instanceId;
}


void CProductInstanceInfo::SetProductInstanceId(const QByteArray &instanceId)
{
	if (m_instanceId != instanceId){
		istd::CChangeNotifier changeNotifier(this);

		m_instanceId = instanceId;
	}
}


QByteArray CProductInstanceInfo::GetCustomerId() const
{
	return m_customerId;
}


QByteArray CProductInstanceInfo::GetSerialNumber() const
{
	return m_serialNumber;
}


void CProductInstanceInfo::SetSerialNumber(const QByteArray& serialNumber)
{
	if (m_serialNumber != serialNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_serialNumber = serialNumber;
	}
}


// reimplemented (imtlic::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CProductInstanceInfo::GetLicenseInstances() const
{
	return m_licenseContainerInfo;
}


const imtlic::ILicenseInstance* CProductInstanceInfo::GetLicenseInstance(const QByteArray& licenseId) const
{
	if (m_licenses.contains(licenseId)){
		return m_licenses[licenseId].GetPtr();
	}

	return nullptr;
}


// reimplemented (iser::ISerializable)

bool CProductInstanceInfo::Serialize(iser::IArchive& archive)
{
	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();

	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	if (imtCoreVersion >= 7003 || imtCoreVersion == 0){
		static iser::CArchiveTag serialNumberTag("SerialNumber", "Serial Number of the product", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(serialNumberTag);
		retVal = retVal && archive.Process(m_serialNumber);
		retVal = retVal && archive.EndTag(serialNumberTag);
	}

	static iser::CArchiveTag instanceIdTag("InstanceId", "ID of the product instance", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(instanceIdTag);
	retVal = retVal && archive.Process(m_instanceId);
	retVal = retVal && archive.EndTag(instanceIdTag);

	static iser::CArchiveTag customerIdTag("CustomerId", "ID of the customer", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(customerIdTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(customerIdTag);

	static iser::CArchiveTag licensesTag("Licenses", "List of licenses", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag licenseInstanceTag("LicenseInstance", "License instance", iser::CArchiveTag::TT_GROUP);
	static iser::CArchiveTag licenseTag("LicenseData", "License data", iser::CArchiveTag::TT_GROUP);

	int licensesCount = m_licenses.count();

	retVal = retVal && archive.BeginMultiTag(licensesTag, licenseInstanceTag, licensesCount);

	if (!archive.IsStoring()){
		m_licenses.clear();
		m_licenseContainerInfo.ResetData();
	}

	if (archive.IsStoring()){
		for (LicenseInstances::Iterator iter = m_licenses.begin(); iter != m_licenses.end(); ++iter){
			retVal = retVal && archive.BeginTag(licenseInstanceTag);

			Q_ASSERT(iter.value().IsValid());

			retVal = archive.BeginTag(licenseTag);
			retVal = retVal && iter.value()->Serialize(archive);
			retVal = retVal && archive.EndTag(licenseTag);

			retVal = retVal && archive.EndTag(licenseInstanceTag);
		}
	}
	else{
		for (int i = 0; i < licensesCount; ++i){
			retVal = retVal && archive.BeginTag(licenseInstanceTag);

			LicenseInstancePtr licenseDataPtr(new CLicenseInstance);
			retVal = archive.BeginTag(licenseTag);
			retVal = retVal && licenseDataPtr->Serialize(archive);
			retVal = retVal && archive.EndTag(licenseTag);

			retVal = retVal && archive.EndTag(licenseInstanceTag);

			if (retVal){
				m_licenses[licenseDataPtr->GetLicenseId()] = licenseDataPtr;
				m_licenseContainerInfo.InsertItem(licenseDataPtr->GetLicenseId(), "", "");
			}
		}
	}

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

		m_productId = sourcePtr->m_productId;
		m_serialNumber = sourcePtr->m_serialNumber;
		m_customerId = sourcePtr->m_customerId;
		m_instanceId= sourcePtr->m_instanceId;
		m_licenses = sourcePtr->m_licenses;
		m_licenseContainerInfo= sourcePtr->m_licenseContainerInfo;

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

	m_productId.clear();
	m_serialNumber.clear();
	m_customerId.clear();
	m_instanceId.clear();
	m_licenses.clear();
	m_licenseContainerInfo.ResetData();

	return true;
}


QByteArray CProductInstanceInfo::GetFactoryId() const
{
	return "Software";
}


QByteArray CProductInstanceInfo::FindProductByName(const QString& productName) const
{
	if (m_productCollectionPtr != nullptr){
		imtbase::ICollectionInfo::Ids productCollectionIds = m_productCollectionPtr->GetElementIds();
		for (const QByteArray productCollectionId : productCollectionIds){
			if (m_productCollectionPtr->GetElementInfo(productCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString() == productName){
				return productCollectionId;
			}
		}
	}

	return QByteArray();
}


} // namespace imtlic


