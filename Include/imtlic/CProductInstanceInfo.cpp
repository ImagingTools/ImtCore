// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductInstanceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/imtbase.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlic
{


// public methods

CProductInstanceInfo::CProductInstanceInfo():
	m_customerCollectionPtr(nullptr),
	m_productCollectionPtr(nullptr),
	m_inUse(false),
	m_internalUse(false),
	m_isMultiProduct(false),
	m_productCount(1)
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
	m_licenseContainerInfo.ResetData();
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


QByteArray CProductInstanceInfo::GetProject() const
{
	return m_project;
}


void CProductInstanceInfo::SetProject(const QByteArray& project)
{
	if (m_project != project){
		istd::CChangeNotifier changeNotifier(this);

		m_project = project;
	}
}


bool CProductInstanceInfo::IsInUse() const
{
	return m_inUse;
}


void CProductInstanceInfo::SetInUse(bool inUse)
{
	if (m_inUse != inUse){
		istd::CChangeNotifier changeNotifier(this);

		m_inUse = inUse;
	}
}


bool CProductInstanceInfo::IsInternalUse() const
{
	return m_internalUse;
}


void CProductInstanceInfo::SetInternalUse(bool internalUse)
{
	if (m_internalUse != internalUse){
		istd::CChangeNotifier changeNotifier(this);

		m_internalUse = internalUse;
	}
}


bool CProductInstanceInfo::IsMultiProduct() const
{
	return m_isMultiProduct;
}


void CProductInstanceInfo::SetMultiProduct(bool isMulti)
{
	if (m_isMultiProduct != isMulti){
		istd::CChangeNotifier changeNotifier(this);

		m_isMultiProduct = isMulti;
	}
}


int CProductInstanceInfo::GetProductCount() const
{
	return m_productCount;
}


void CProductInstanceInfo::SetProductCount(int count)
{
	if (m_productCount != count){
		istd::CChangeNotifier changeNotifier(this);

		m_productCount = count;
	}
}


QByteArray CProductInstanceInfo::GetParentInstanceId() const
{
	return m_parentInstanceId;
}


void CProductInstanceInfo::SetParentInstanceId(const QByteArray& parentInstanceId)
{
	if (m_parentInstanceId != parentInstanceId){
		istd::CChangeNotifier changeNotifier(this);

		m_parentInstanceId = parentInstanceId;
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

	iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	if (imtCoreVersion >= 7003 || imtCoreVersion == 0){
		iser::CArchiveTag serialNumberTag("SerialNumber", "Serial Number of the product", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(serialNumberTag);
		retVal = retVal && archive.Process(m_serialNumber);
		retVal = retVal && archive.EndTag(serialNumberTag);
	}

	if (imtCoreVersion >= 7386){
		iser::CArchiveTag inUseTag("InUse", "In-Use", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(inUseTag);
		retVal = retVal && archive.Process(m_inUse);
		retVal = retVal && archive.EndTag(inUseTag);
	}

	if (imtCoreVersion >= 15177){
		iser::CArchiveTag internalUseTag("InternalUse", "Internal Use", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(internalUseTag);
		retVal = retVal && archive.Process(m_internalUse);
		retVal = retVal && archive.EndTag(internalUseTag);
	}

	if (imtCoreVersion >= 7386){
		iser::CArchiveTag projectTag("Project", "Project", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(projectTag);
		retVal = retVal && archive.Process(m_project);
		retVal = retVal && archive.EndTag(projectTag);
	}

	if (imtCoreVersion >= 19722){
		iser::CArchiveTag isMultiTag("IsMultiProduct", "Is multi product", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(isMultiTag);
		retVal = retVal && archive.Process(m_isMultiProduct);
		retVal = retVal && archive.EndTag(isMultiTag);

		iser::CArchiveTag productCountTag("ProductCount", "Product count", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(productCountTag);
		retVal = retVal && archive.Process(m_productCount);
		retVal = retVal && archive.EndTag(productCountTag);
	}

	if (imtCoreVersion >= 19900){
		iser::CArchiveTag parentInstanceIdTag("ParentInstanceId", "Parent instance ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(parentInstanceIdTag);
		retVal = retVal && archive.Process(m_parentInstanceId);
		retVal = retVal && archive.EndTag(parentInstanceIdTag);
	}

	iser::CArchiveTag instanceIdTag("InstanceId", "ID of the product instance", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(instanceIdTag);
	retVal = retVal && archive.Process(m_instanceId);
	retVal = retVal && archive.EndTag(instanceIdTag);

	iser::CArchiveTag customerIdTag("CustomerId", "ID of the customer", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(customerIdTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(customerIdTag);

	iser::CArchiveTag licensesTag("Licenses", "List of licenses", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag licenseInstanceTag("LicenseInstance", "License instance", iser::CArchiveTag::TT_GROUP);
	iser::CArchiveTag licenseTag("LicenseData", "License data", iser::CArchiveTag::TT_GROUP);

	int licensesCount = imtbase::narrow_cast<int>(m_licenses.size());

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
		m_project = sourcePtr->m_project;
		m_customerId = sourcePtr->m_customerId;
		m_instanceId= sourcePtr->m_instanceId;
		m_licenses = sourcePtr->m_licenses;
		m_licenseContainerInfo = sourcePtr->m_licenseContainerInfo;
		m_inUse = sourcePtr->m_inUse;
		m_internalUse = sourcePtr->m_internalUse;
		m_isMultiProduct = sourcePtr->m_isMultiProduct;
		m_productCount = sourcePtr->m_productCount;
		m_parentInstanceId = sourcePtr->m_parentInstanceId;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CProductInstanceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CProductInstanceInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CProductInstanceInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productId.clear();
	m_serialNumber.clear();
	m_project.clear();
	m_customerId.clear();
	m_instanceId.clear();
	m_licenses.clear();
	m_licenseContainerInfo.ResetData();
	m_inUse = false;
	m_internalUse = false;
	m_isMultiProduct = false;
	m_productCount = 0;
	m_parentInstanceId.clear();

	return true;
}


QByteArray CProductInstanceInfo::GetFactoryId() const
{
	return "Software";
}


} // namespace imtlic