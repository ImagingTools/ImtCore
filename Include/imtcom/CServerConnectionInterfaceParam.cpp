// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CServerConnectionInterfaceParam.h>


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtcom
{


CServerConnectionInterfaceParam::CServerConnectionInterfaceParam()
{
}


void CServerConnectionInterfaceParam::RegisterProtocol(ProtocolType protocol)
{
	if (!m_interfaceMap.contains(protocol)){
		istd::CChangeNotifier changeNotifier(this);

		m_interfaceMap[protocol] = -1;
	}
}


// reimplemented (imtcom::IServerConnectionInterface)

int CServerConnectionInterfaceParam::GetConnectionFlags() const
{
	return m_connectionFlags;
}


void CServerConnectionInterfaceParam::SetConnectionFlags(int connectionFlags)
{
	if (m_connectionFlags != connectionFlags){
		istd::CChangeNotifier changeNotifier(this);

		m_connectionFlags = connectionFlags;
	}
}


QString CServerConnectionInterfaceParam::GetHost() const
{
	return m_host;
}


void CServerConnectionInterfaceParam::SetHost(const QString& host)
{
	if (m_host != host){
		istd::CChangeNotifier changeNotifier(this);

		m_host = host;
	}
}


int CServerConnectionInterfaceParam::GetPort(ProtocolType protocol) const
{
	if (m_interfaceMap.contains(protocol)){
		return m_interfaceMap[protocol];
	}

	return -1;
}


void CServerConnectionInterfaceParam::SetPort(ProtocolType protocol, int port)
{
	if (m_interfaceMap.contains(protocol)){
		istd::CChangeNotifier changeNotifier(this);

		m_interfaceMap[protocol] = port;
	}
	else{
		Q_ASSERT_X(false, "CServerConnectionInterfaceParam::SetPort", "Protocol was not registered");
	}
}


IServerConnectionInterface::ProtocolTypes CServerConnectionInterfaceParam::GetSupportedProtocols() const
{
	return m_interfaceMap.keys();
}


bool CServerConnectionInterfaceParam::GetUrl(ProtocolType protocol, QUrl& url) const
{
	int port = GetPort(protocol);

	QString scheme;
	switch (protocol){
	case PT_HTTP:
		scheme = GetConnectionFlags() & CF_SECURE ? "https" : "http";
		break;
	case PT_WEBSOCKET:
		scheme = GetConnectionFlags() & CF_SECURE ? "wss" : "ws";
		break;
	case PT_FILE:
		scheme = "file";
		break;
	default:
		Q_ASSERT_X(false, "CServerConnectionInterfaceParam::GetUrl", "Unsupported protocol");

		break;
	}

	url.setScheme(scheme);
	url.setHost(m_host);
	url.setPort(port);

	return url.isValid();
}


// reimplemented (iser::ISerializable)

bool CServerConnectionInterfaceParam::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag hostTag("Host", "Name of the host");
	retVal = retVal && archive.BeginTag(hostTag);
	retVal = retVal && archive.Process(m_host);
	retVal = retVal && archive.EndTag(hostTag);

	static iser::CArchiveTag connectionFlagsTag("ConnectionFlags", "Connection settings");
	retVal = retVal && archive.BeginTag(connectionFlagsTag);
	retVal = retVal && I_SERIALIZE_FLAG(ConnectionFlags, archive, m_connectionFlags);
	retVal = retVal && archive.EndTag(connectionFlagsTag);

	auto keySerializer = [](iser::IArchive& archive, ProtocolType& protocolType){
		return iser::CPrimitiveTypesSerializer::SerializeEnum<ProtocolType, IServerConnectionInterface::ToString, IServerConnectionInterface::FromString>(archive, protocolType);
	};

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeAssociativeContainer<InterfaceMap, ProtocolType>(
				archive,
				m_interfaceMap,
				keySerializer,
				"Interfaces",
				"Interface",
				"Protocol",
				"Port",
				"List of interfaces");

	return retVal;
}


// reimplemented (istd::IChangeable)

int CServerConnectionInterfaceParam::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET | SO_COMPARE;
}


bool CServerConnectionInterfaceParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CServerConnectionInterfaceParam* sourcePtr = dynamic_cast<const CServerConnectionInterfaceParam*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_interfaceMap = sourcePtr->m_interfaceMap;
		m_connectionFlags = sourcePtr->m_connectionFlags;
		m_host = sourcePtr->m_host;

		return true;
	}

	return false;
}


bool CServerConnectionInterfaceParam::IsEqual(const IChangeable& object) const
{
	const CServerConnectionInterfaceParam* sourcePtr = dynamic_cast<const CServerConnectionInterfaceParam*>(&object);
	if (sourcePtr != NULL){
		bool retVal = m_interfaceMap == sourcePtr->m_interfaceMap;
		retVal = retVal && m_connectionFlags == sourcePtr->m_connectionFlags;
		retVal = retVal && m_host == sourcePtr->m_host;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CServerConnectionInterfaceParam::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CServerConnectionInterfaceParam> retVal(new CServerConnectionInterfaceParam);
	if (retVal->CopyFrom(*this, mode)){
		return retVal.PopPtr();
	}

	return nullptr;
}


bool CServerConnectionInterfaceParam::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_interfaceMap.clear();
	m_connectionFlags = CF_DEFAULT;
	m_host.clear();

	return true;
}


} // namespace imtcom


