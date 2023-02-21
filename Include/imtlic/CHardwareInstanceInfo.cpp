#include <imtlic/CHardwareInstanceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtlic
{


// public methods

CHardwareInstanceInfo::CHardwareInstanceInfo()
{
}


QByteArray CHardwareInstanceInfo::GetProductId() const
{
	return m_productId;
}


void CHardwareInstanceInfo::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = productId;
	}
}


// reimplemented (imtlic::ILicensedHardwareInstanceInfo)

QByteArray CHardwareInstanceInfo::GetDeviceId() const
{
	return m_deviceId;
}

void CHardwareInstanceInfo::SetDeviceId(const QByteArray &deviceId)
{
	if (m_deviceId != deviceId){
		istd::CChangeNotifier changeNotifier(this);

		m_deviceId = deviceId;
	}
}


QByteArray CHardwareInstanceInfo::GetSoftwareId() const
{
	return m_softwareId;
}


void CHardwareInstanceInfo::SetSoftwareId(const QByteArray& softwareId)
{
	if (m_softwareId != softwareId){
		istd::CChangeNotifier changeNotifier(this);

		m_softwareId = softwareId;
	}
}


// reimplemented (iser::ISerializable)

bool CHardwareInstanceInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag productIdTag("ProductId", "Product-ID", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	static iser::CArchiveTag deviceIdTag("DeviceId", "Device-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(deviceIdTag);
	retVal = retVal && archive.Process(m_deviceId);
	retVal = retVal && archive.EndTag(deviceIdTag);

	static iser::CArchiveTag softwareIdTag("SoftwareId", "ID of the license software", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(softwareIdTag);
	retVal = retVal && archive.Process(m_softwareId);
	retVal = retVal && archive.EndTag(softwareIdTag);

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 archiveVersion;
	if (!versionInfo.GetVersionNumber(2022, archiveVersion)){
		archiveVersion = 0;
	}

	if (archiveVersion == 0) {
		imtlic::CProductInstanceInfo info;
		retVal = retVal && info.Serialize(archive); // ?????????????????
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CHardwareInstanceInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CHardwareInstanceInfo* sourcePtr = dynamic_cast<const imtlic::CHardwareInstanceInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_deviceId = sourcePtr->m_deviceId;
		m_softwareId = sourcePtr->m_softwareId;
		m_productId = sourcePtr->m_productId;

		return true;
	}

	return false;
}


istd::IChangeable* CHardwareInstanceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CHardwareInstanceInfo> clonePtr(new CHardwareInstanceInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CHardwareInstanceInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_deviceId.clear();
	m_softwareId.clear();
	m_productId.clear();

	return true;
}


QByteArray CHardwareInstanceInfo::GetFactoryId() const
{
	return "Hardware";
}


} // namespace imtlic


