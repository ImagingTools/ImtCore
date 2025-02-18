#include <imtservice/CServiceConnectionInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtservice
{


// public methods

CServiceConnectionInfo::CServiceConnectionInfo()
	:m_connectionStatus(CS_OK),
	m_connectionType(CT_INPUT)
{
}


void CServiceConnectionInfo::SetConnectionType(imtservice::IServiceConnectionInfo::ConnectionType connectionType)
{
	if (m_connectionType != connectionType){
		istd::CChangeNotifier changeNotifier(this);

		m_connectionType = connectionType;
	}
}


void CServiceConnectionInfo::SetServiceTypeName(const QByteArray& serviceTypeName)
{
	if (m_serviceTypeName != serviceTypeName){
		istd::CChangeNotifier changeNotifier(this);

		m_serviceTypeName = serviceTypeName;
	}
}


void CServiceConnectionInfo::SetUsageId(const QByteArray& usageId)
{
	if (m_usageId != usageId){
		istd::CChangeNotifier changeNotifier(this);

		m_usageId = usageId;
	}
}


void CServiceConnectionInfo::SetDefaultUrl(const QUrl& url)
{
	if (m_defaultUrl != url){
		istd::CChangeNotifier changeNotifier(this);
		
		m_defaultUrl = url;
	}
}


// reimplemented (imtservice::IServiceConnectionParam)

imtservice::IServiceConnectionInfo::ConnectionType CServiceConnectionInfo::GetConnectionType() const
{
	return m_connectionType;
}


QByteArray CServiceConnectionInfo::GetServiceTypeName() const
{
	return m_serviceTypeName;
}


QByteArray CServiceConnectionInfo::GetUsageId() const
{
	return m_usageId;
}


QUrl CServiceConnectionInfo::GetDefaultUrl() const
{
	return m_defaultUrl;
}


// reimplemented (imtservice::IConnectionStatus)

imtservice::IConnectionStatus::ConnectionStatus CServiceConnectionInfo::GetConnectionStatus() const
{
	return m_connectionStatus;
}


// reimplemented (iser::ISerializable)

bool CServiceConnectionInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	retVal = retVal && BaseClass::Serialize(archive);

	iser::CArchiveTag serviceTypeNameTag("ServiceTypeName", "Service TypeName", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceTypeNameTag);
	retVal = retVal && archive.Process(m_serviceTypeName);
	retVal = retVal && archive.EndTag(serviceTypeNameTag);

	iser::CArchiveTag serviceUsageIdTag("UsageId", "UsageId", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceUsageIdTag);
	retVal = retVal && archive.Process(m_usageId);
	retVal = retVal && archive.EndTag(serviceUsageIdTag);

	iser::CArchiveTag connectionTypeTag("ConnectionType", "Connection Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionTypeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
				 IServiceConnectionInfo::ConnectionType,
				 IServiceConnectionInfo::ToString,
				 IServiceConnectionInfo::FromString>(archive, m_connectionType);
	retVal = retVal && archive.EndTag(connectionTypeTag);

	iser::CArchiveTag defaultUrlTag("DefaultUrl", "DefaultUrl", iser::CArchiveTag::TT_LEAF);
	QString defaultUrlStr = m_defaultUrl.toString();

	retVal = retVal && archive.BeginTag(defaultUrlTag);
	retVal = retVal && archive.Process(defaultUrlStr);
	retVal = retVal && archive.EndTag(defaultUrlTag);

	if (retVal && !archive.IsStoring()){
		m_defaultUrl = QUrl(defaultUrlStr);
	}
	
	iser::CArchiveTag connectionStatusTag("ConnectionStatus", "Connection Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionStatusTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
				 IConnectionStatus::ConnectionStatus,
				 IConnectionStatus::ToString,
				 IConnectionStatus::FromString>(archive, m_connectionStatus);
	retVal = retVal && archive.EndTag(connectionStatusTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CServiceConnectionInfo::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	const CServiceConnectionInfo* sourcePtr = dynamic_cast<const CServiceConnectionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		if (!BaseClass::CopyFrom(object, mode)){
			return false;
		}

		m_serviceTypeName = sourcePtr->m_serviceTypeName;
		m_usageId = sourcePtr->m_usageId;
		m_connectionType = sourcePtr->m_connectionType;
		m_defaultUrl = sourcePtr->m_defaultUrl;
		m_connectionStatus = sourcePtr->m_connectionStatus;

		return true;
	}

	return false;
}


istd::IChangeable* CServiceConnectionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CServiceConnectionInfo> clonePtr(new CServiceConnectionInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CServiceConnectionInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);
	
	bool retVal = BaseClass::ResetData(mode);

	m_serviceTypeName.clear();
	m_usageId.clear();
	m_connectionType = CT_INPUT;
	m_defaultUrl.clear();
	m_connectionStatus = CS_OK;

	return retVal;
}


} // namespace agentinodata


