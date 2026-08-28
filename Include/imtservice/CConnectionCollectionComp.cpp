// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservice/CConnectionCollectionComp.h>


// ImtCore includes
#include <imtservice/CUrlConnectionParam.h>


namespace imtservice
{


namespace
{


/**
	Copy host/ports/path/flags through the IServerConnectionInterface accessors.

	Unlike IChangeable::CopyFrom(), which most implementations (e.g. CUrlConnectionParam)
	restrict to their own exact concrete type, every implementation of
	IServerConnectionInterface supports these setters directly, so this works between
	unrelated concrete classes (e.g. copying a CServerConnectionInterfaceParam built by a
	caller into a CUrlConnectionParam held by this collection).
*/
void CopyServerConnectionInterface(imtcom::IServerConnectionInterface& target, const imtcom::IServerConnectionInterface& source)
{
	target.SetHost(source.GetHost());
	target.SetConnectionFlags(source.GetConnectionFlags());

	const imtcom::IServerConnectionInterface::ProtocolTypes protocols = source.GetSupportedProtocols();
	for (imtcom::IServerConnectionInterface::ProtocolType protocol : protocols){
		target.SetPort(protocol, source.GetPort(protocol));
		target.SetPath(protocol, source.GetPath(protocol));
	}
}


} // namespace


// public methods

// reimplemented (imtservice::IConnectionCollection)

QString CConnectionCollectionComp::GetServiceVersion() const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return QString();
	}

	QString version = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_MAIN_VERSION);

	return version;
}

QString CConnectionCollectionComp::GetServiceTypeId() const
{
	QString retVal;
	if (m_serviceTypeId.IsValid()){
		retVal = m_serviceTypeId->GetValue();
	}

	return retVal;
}


bool CConnectionCollectionComp::SetServiceTypeId(const QString& /*serviceTypeName*/) const
{
	return false;
}


const imtbase::ICollectionInfo* CConnectionCollectionComp::GetServerConnectionList() const
{
	return &m_collection;
}

const imtcom::IServerConnectionInterface* CConnectionCollectionComp::GetServerConnection(const QByteArray& id) const
{
	return dynamic_cast<imtcom::IServerConnectionInterface*>(const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(id)));
}


const IServiceConnectionInfo* CConnectionCollectionComp::GetConnectionMetaInfo(const QByteArray& id) const
{
	CUrlConnectionParam* urlConnectionParam =  dynamic_cast<CUrlConnectionParam*>(const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(id)));
	if (urlConnectionParam != nullptr){

		return urlConnectionParam;
	}

	return nullptr;
}


bool CConnectionCollectionComp::SetServerConnectionInterface(const QByteArray& connectionId, const imtcom::IServerConnectionInterface& connectionInterface)
{
	const istd::IChangeable* objectPtr = m_collection.GetObjectPtr(connectionId);
	if (objectPtr == nullptr){
		return false;
	}

	istd::IChangeable* notConstObjectPtr = const_cast<istd::IChangeable*>(objectPtr);
	if (notConstObjectPtr == nullptr){
		return false;
	}

	imtcom::IServerConnectionInterface* urlConnectionParam =  dynamic_cast<imtcom::IServerConnectionInterface*>(notConstObjectPtr);
	if (urlConnectionParam != nullptr){
		imtbase::ICollectionInfo::Ids elementIds = m_collection.GetElementIds();
		for (int index = 0; index < elementIds.size(); index++){
			const imtbase::ICollectionInfo::Id& elementId = elementIds[index];
			if (elementId == connectionId){
				if (index < m_serverInterfaceListCompPtr.GetCount()){
					// Not CopyFrom(): it requires an exact concrete-type match (e.g.
					// CUrlConnectionParam::CopyFrom rejects a CServerConnectionInterfaceParam
					// source), which a caller updating just host/port never has.
					CopyServerConnectionInterface(*m_serverInterfaceListCompPtr[index], connectionInterface);
					CopyServerConnectionInterface(*urlConnectionParam, connectionInterface);

					return true;
				}
			}
		}
	}

	return false;
}


QByteArray CConnectionCollectionComp::InsertNewConnection(
			const QByteArray& /*connectionId*/,
			const QUrl& /*url*/,
			imtservice::IServiceConnectionInfo::ConnectionType /*connectionType*/,
			const QString& /*name*/,
			const QString& /*description*/)
{
	return QByteArray();
}


int CConnectionCollectionComp::GetTracingLevel() const
{
	if (!m_tracingConfigurationCompPtr.IsValid()){
		return -1;
	}

	return m_tracingConfigurationCompPtr->GetTracingLevel();
}


void CConnectionCollectionComp::SetTracingLevel(int tracingLevel)
{
	if (!m_tracingConfigurationCompPtr.IsValid()){
		return;
	}

	m_tracingConfigurationCompPtr->SetTracingLevel(tracingLevel);
}


// reimplemented (icomp::CComponentBase)

void CConnectionCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	typedef istd::TSingleFactory<istd::IChangeable, imtservice::CUrlConnectionParam> FactoryConnectionImpl;
	m_collection.RegisterFactory<FactoryConnectionImpl>("ConnectionInfo");

	if (
		m_serverInterfaceListCompPtr.IsValid() &&
		m_connectionServiceTypeIds.GetCount() == m_connectionNames.GetCount() &&
		m_connectionServiceTypeIds.GetCount() == m_connectionDescriptions.GetCount() &&
		m_connectionServiceTypeIds.GetCount() == m_connectionTypes.GetCount() &&
		m_connectionServiceTypeIds.GetCount() == m_serverInterfaceListCompPtr.GetCount()){
		for (int index = 0; index < m_connectionServiceTypeIds.GetCount(); index++){
			IServiceConnectionInfo::ConnectionType connectionType = IServiceConnectionInfo::CT_INPUT;
			if (m_connectionTypes[index] == 1){
				connectionType = IServiceConnectionInfo::CT_OUTPUT;
			}

			QByteArray name = m_connectionNames[index];
			QByteArray serviceConnectionTypeId = m_connectionServiceTypeIds[index];
			QByteArray description = m_connectionDescriptions[index];

			CUrlConnectionParam urlConnectionParam;
			urlConnectionParam.RegisterProtocol(imtcom::IServerConnectionInterface::PT_HTTP);
			urlConnectionParam.RegisterProtocol(imtcom::IServerConnectionInterface::PT_WEBSOCKET);

			urlConnectionParam.SetServiceTypeId(serviceConnectionTypeId);
			urlConnectionParam.SetConnectionType(connectionType);

			const imtcom::IServerConnectionInterface* connectionInterfacePtr = m_serverInterfaceListCompPtr[index];
			if (connectionInterfacePtr != nullptr){
				urlConnectionParam.SetDefaultServiceInterface(*connectionInterfacePtr);

				QString defaultHost = connectionInterfacePtr->GetHost();
				urlConnectionParam.SetHost(defaultHost);

				int defaultHttpPort = connectionInterfacePtr->GetPort(imtcom::IServerConnectionInterface::PT_HTTP);
				urlConnectionParam.SetPort(imtcom::IServerConnectionInterface::PT_HTTP, defaultHttpPort);

				int defaultWsPort = connectionInterfacePtr->GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET);
				urlConnectionParam.SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, defaultWsPort);
			}

			QByteArray retVal = m_collection.InsertNewObject("ConnectionInfo", name, description, &urlConnectionParam, serviceConnectionTypeId);
			if (retVal.isEmpty()){
				SendErrorMessage(
					0,
					QStringLiteral("Unable to insert connection '%1' to collection. Insert to collection failed").arg(serviceConnectionTypeId),
					"CConnectionCollectionComp");
			}
		}
	}
}


} // namespace agentinodata


