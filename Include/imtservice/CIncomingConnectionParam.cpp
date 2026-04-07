// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservice/CIncomingConnectionParam.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtservice
{


// public methods

QString CIncomingConnectionParam::GetHost() const
{
	return m_host;
}


void CIncomingConnectionParam::SetHost(const QString& host)
{
	if (m_host != host){
		istd::CChangeNotifier changeNotifier(this);

		m_host = host;
	}
}


QString CIncomingConnectionParam::GetDescription() const
{
	return m_description;
}


void CIncomingConnectionParam::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


int CIncomingConnectionParam::GetWsPort() const
{
	return m_wsPort;
}


void CIncomingConnectionParam::SetWsPort(int wsPort)
{
	if (m_wsPort != wsPort){
		istd::CChangeNotifier changeNotifier(this);

		m_wsPort = wsPort;
	}
}


int CIncomingConnectionParam::GetHttpPort() const
{
	return m_httpPort;
}


void CIncomingConnectionParam::SetHttpPort(int httpPort)
{
	if (m_httpPort != httpPort){
		istd::CChangeNotifier changeNotifier(this);

		m_httpPort = httpPort;
	}
}


// reimplemented (iser::ISerializable)

bool CIncomingConnectionParam::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	QString host = m_host;
	QString description = m_description;
	int wsPort = m_wsPort;
	int httpPort = m_httpPort;

	iser::CArchiveTag descriptionTag("Description", "Connection description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag hostTag("Host", "Host", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(hostTag);
	retVal = retVal && archive.Process(host);
	retVal = retVal && archive.EndTag(hostTag);

	iser::CArchiveTag wsPortTag("WsPort", "Web Socket Port", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(wsPortTag);
	retVal = retVal && archive.Process(wsPort);
	retVal = retVal && archive.EndTag(wsPortTag);

	iser::CArchiveTag httpPortTag("HttpPort", "Http Socket Port", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(httpPortTag);
	retVal = retVal && archive.Process(httpPort);
	retVal = retVal && archive.EndTag(httpPortTag);

	if (retVal && !archive.IsStoring()){
		m_host = host;
		m_description = description;
		m_wsPort = wsPort;
		m_httpPort = httpPort;
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CIncomingConnectionParam::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CIncomingConnectionParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CIncomingConnectionParam* sourcePtr = dynamic_cast<const CIncomingConnectionParam*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_host = sourcePtr->m_host;
		m_description = sourcePtr->m_description;
		m_wsPort = sourcePtr->m_wsPort;
		m_httpPort = sourcePtr->m_httpPort;

		return true;
	}

	return false;
}


bool CIncomingConnectionParam::IsEqual(const istd::IChangeable& object) const
{
	const CIncomingConnectionParam* sourcePtr = dynamic_cast<const CIncomingConnectionParam*>(&object);
	if (sourcePtr != nullptr){
		return	(m_host == sourcePtr->m_host) &&
				(m_description == sourcePtr->m_description) &&
				(m_wsPort == sourcePtr->m_wsPort) &&
				(m_httpPort == sourcePtr->m_httpPort);
	}

	return false;
}


istd::IChangeableUniquePtr CIncomingConnectionParam::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CIncomingConnectionParam());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CIncomingConnectionParam::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_host = QStringLiteral("localhost");
	m_description.clear();
	m_wsPort = -1;
	m_httpPort = -1;

	return true;
}


} // namespace imtservice


