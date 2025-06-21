#include <imtservice/CConnectionCollectionComp.h>


// ImtCore includes
#include <imtservice/CUrlConnectionParam.h>


namespace imtservice
{


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

QString CConnectionCollectionComp::GetServiceTypeName() const
{
	QString retVal;
	if (m_serviceTypeName.IsValid()){
		retVal = m_serviceTypeName->GetValue();
	}

	return retVal;
}


bool CConnectionCollectionComp::SetServiceTypeName(const QString& /*serviceTypeName*/) const
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


bool CConnectionCollectionComp::SetServerConnectionInterface(const QByteArray& id, const imtcom::IServerConnectionInterface& connectionInterface)
{
	const istd::IChangeable* objectPtr = m_collection.GetObjectPtr(id);
	if (objectPtr == nullptr){
		return false;
	}

	istd::IChangeable* notConstObjectPtr = const_cast<istd::IChangeable*>(objectPtr);
	if (notConstObjectPtr == nullptr){
		return false;
	}

	imtcom::IServerConnectionInterface* urlConnectionParam =  dynamic_cast<imtcom::IServerConnectionInterface*>(notConstObjectPtr);
	if (urlConnectionParam != nullptr){
		bool retVal = true;

		for (int index = 0; index < m_connectionUsageIds.GetCount(); index++){
			if (id == m_connectionUsageIds[index]){
				if (index < m_serverInterfaceListCompPtr.GetCount()){
					retVal = retVal && m_serverInterfaceListCompPtr[index]->CopyFrom(connectionInterface);

					retVal = retVal && urlConnectionParam->CopyFrom(connectionInterface);

					break;
				}
			}
		}

		return retVal;
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
	typedef istd::TSingleFactory<istd::IChangeable, imtservice::CUrlConnectionParam> FactoryConnectionImpl;
	m_collection.RegisterFactory<FactoryConnectionImpl>("ConnectionInfo");

	if (
		m_serverInterfaceListCompPtr.IsValid() &&
		m_connectionUsageIds.GetCount() == m_connectionNames.GetCount() &&
		m_connectionUsageIds.GetCount() == m_connectionServiceTypeNames.GetCount() &&
		m_connectionUsageIds.GetCount() == m_connectionDescriptions.GetCount() &&
		m_connectionUsageIds.GetCount() == m_connectionTypes.GetCount() &&
		m_connectionUsageIds.GetCount() == m_serverInterfaceListCompPtr.GetCount()){
		for (int index = 0; index < m_connectionUsageIds.GetCount(); index++){
			IServiceConnectionInfo::ConnectionType connectionType = IServiceConnectionInfo::CT_INPUT;
			if (m_connectionTypes[index] == 1){
				connectionType = IServiceConnectionInfo::CT_OUTPUT;
			}
			QByteArray name = m_connectionNames[index];
			QByteArray serviceTypeName = m_connectionServiceTypeNames[index];
			QByteArray connectionUsageId = m_connectionUsageIds[index];
			QByteArray description = m_connectionDescriptions[index];

			CUrlConnectionParam urlConnectionParam;
			urlConnectionParam.SetServiceTypeName(serviceTypeName);
			urlConnectionParam.SetUsageId(connectionUsageId);
			urlConnectionParam.SetConnectionType(connectionType);
			urlConnectionParam.SetDefaultServiceInterface(*m_serverInterfaceListCompPtr[index]);
			
			QByteArray retVal = m_collection.InsertNewObject("ConnectionInfo", name, description, &urlConnectionParam, connectionUsageId);
			if (retVal.isEmpty()){
				SendErrorMessage(
					0,
					QString("Unable to insert connection '%1' to collection. Insert to collection failed").arg(qPrintable(connectionUsageId)),
					"CConnectionCollectionComp");
			}
		}
	}
}


} // namespace agentinodata


