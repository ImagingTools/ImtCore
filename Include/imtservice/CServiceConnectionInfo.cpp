// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservice/CServiceConnectionInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtservice
{


// public methods

CServiceConnectionInfo::CServiceConnectionInfo()
	:m_connectionType(CT_INPUT),
	m_connectionStatus(CS_OK)
{
}


void CServiceConnectionInfo::SetConnectionType(imtservice::IServiceConnectionInfo::ConnectionType connectionType)
{
	if (m_connectionType != connectionType){
		istd::CChangeNotifier changeNotifier(this);

		m_connectionType = connectionType;
	}
}


void CServiceConnectionInfo::SetServiceTypeId(const QByteArray& serviceTypeId)
{
	if (m_serviceTypeId != serviceTypeId){
		istd::CChangeNotifier changeNotifier(this);

		m_serviceTypeId = serviceTypeId;
	}
}


void CServiceConnectionInfo::SetDefaultServiceInterface(const imtcom::IServerConnectionInterface& defaultInterface)
{
	if (!m_defaultConnection.IsEqual(defaultInterface)){
		istd::CChangeNotifier changeNotifier(this);

		m_defaultConnection.CopyFrom(defaultInterface);
	}
}


// reimplemented (imtservice::IServiceConnectionParam)

imtservice::IServiceConnectionInfo::ConnectionType CServiceConnectionInfo::GetConnectionType() const
{
	return m_connectionType;
}


QByteArray CServiceConnectionInfo::GetServiceTypeId() const
{
	return m_serviceTypeId;
}


const imtcom::IServerConnectionInterface& CServiceConnectionInfo::GetDefaultInterface() const
{
	return m_defaultConnection;
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

	bool retVal = BaseClass::Serialize(archive);

	iser::CArchiveTag defaultConnectionInterfaceTag("DefaultConnectionInterface", "Default interface for the service connection", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(defaultConnectionInterfaceTag);
	retVal = retVal && m_defaultConnection.Serialize(archive);
	retVal = retVal && archive.EndTag(defaultConnectionInterfaceTag);

	iser::CArchiveTag serviceTypeNameTag("ServiceTypeName", "Service TypeName", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serviceTypeNameTag);
	retVal = retVal && archive.Process(m_serviceTypeId);
	retVal = retVal && archive.EndTag(serviceTypeNameTag);

	iser::CArchiveTag connectionTypeTag("ConnectionType", "Connection Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionTypeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
				 IServiceConnectionInfo::ConnectionType,
				 IServiceConnectionInfo::ToString,
				 IServiceConnectionInfo::FromString>(archive, m_connectionType);
	retVal = retVal && archive.EndTag(connectionTypeTag);
	
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

		m_serviceTypeId = sourcePtr->m_serviceTypeId;
		m_connectionType = sourcePtr->m_connectionType;
		m_defaultConnection.CopyFrom(sourcePtr->m_defaultConnection);
		m_connectionStatus = sourcePtr->m_connectionStatus;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CServiceConnectionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CServiceConnectionInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CServiceConnectionInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_serviceTypeId.clear();
	m_connectionType = CT_INPUT;
	m_defaultConnection.ResetData();
	m_connectionStatus = CS_OK;

	return BaseClass::ResetData(mode);
}


} // namespace imtservice


