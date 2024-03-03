#include <imtservice/CUrlConnectionLinkParam.h>


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

CUrlConnectionLinkParam::CUrlConnectionLinkParam()
	:m_connectionStatus(CS_OK)
{
}


CUrlConnectionLinkParam::CUrlConnectionLinkParam(const QByteArray& serviceTypeName, const QByteArray& usageId, const QByteArray& dependantServiceConnectionId)
	:m_serviceTypeName(serviceTypeName),
	m_usageId(usageId),
	m_dependantServiceConnectionId(dependantServiceConnectionId),
	m_connectionStatus(CS_OK)
{
}


// reimplemented (imtservice::IServiceConnectionInfo)

imtservice::IServiceConnectionInfo::ConnectionType CUrlConnectionLinkParam::GetConnectionType() const
{
	return CT_OUTPUT;
}


QByteArray CUrlConnectionLinkParam::GetServiceTypeName() const
{
	return m_serviceTypeName;
}


QByteArray CUrlConnectionLinkParam::GetUsageId() const
{
	return m_usageId;
}


QUrl CUrlConnectionLinkParam::GetDefaultUrl() const
{
	return m_defaultUrl;
}


// reimplemented (imtservice::IServiceConnectionLinkParam)

QByteArray CUrlConnectionLinkParam::GetDependantServiceConnectionId() const
{
	return m_dependantServiceConnectionId;
}


// reimplemented (imtservice::IConnectionStatus)

imtservice::IConnectionStatus::ConnectionStatus CUrlConnectionLinkParam::GetConnectionStatus() const
{
	return m_connectionStatus;
}


// reimplemented (iser::ISerializable)

bool CUrlConnectionLinkParam::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag serviceTypeNameTag("ServiceTypeName", "Service TypeName", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceTypeNameTag);
	retVal = retVal && archive.Process(m_serviceTypeName);
	retVal = retVal && archive.EndTag(serviceTypeNameTag);

	iser::CArchiveTag serviceUsageIdTag("UsageId", "UsageId", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceUsageIdTag);
	retVal = retVal && archive.Process(m_usageId);
	retVal = retVal && archive.EndTag(serviceUsageIdTag);

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

	iser::CArchiveTag dependantServiceConnectionIdTag("DependantServiceConnectionId", "Dependant service Connection-Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(dependantServiceConnectionIdTag);
	retVal = retVal && archive.Process(m_dependantServiceConnectionId);
	retVal = retVal && archive.EndTag(dependantServiceConnectionIdTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CUrlConnectionLinkParam::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	const CUrlConnectionLinkParam* sourcePtr = dynamic_cast<const CUrlConnectionLinkParam*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_serviceTypeName = sourcePtr->m_serviceTypeName;
		m_usageId = sourcePtr->m_usageId;
		m_dependantServiceConnectionId = sourcePtr->m_dependantServiceConnectionId;
		m_connectionStatus = sourcePtr->m_connectionStatus;
		m_defaultUrl = sourcePtr->m_defaultUrl;

		return true;
	}

	return false;
}


istd::IChangeable* CUrlConnectionLinkParam::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CUrlConnectionLinkParam> clonePtr(new CUrlConnectionLinkParam);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUrlConnectionLinkParam::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_serviceTypeName.clear();
	m_usageId.clear();
	m_dependantServiceConnectionId.clear();
	m_connectionStatus = CS_OK;
	m_defaultUrl.clear();

	return true;
}


} // namespace agentinodata


