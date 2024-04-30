#include <imtservice/CConnectionCollectionComp.h>


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


const imtbase::ICollectionInfo* CConnectionCollectionComp::GetUrlList() const
{
	return &m_collection;
}


const QUrl* CConnectionCollectionComp::GetUrl(const QByteArray& id) const
{
	CUrlConnectionParam* urlConnectionParam =  dynamic_cast<CUrlConnectionParam*>(const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(id)));
	if (urlConnectionParam != nullptr){
		QUrl* url = new QUrl(urlConnectionParam->GetUrl());

		return url;
	}

	return nullptr;
}


const IServiceConnectionParam* CConnectionCollectionComp::GetConnectionMetaInfo(const QByteArray& id) const
{
	CUrlConnectionParam* urlConnectionParam =  dynamic_cast<CUrlConnectionParam*>(const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(id)));
	if (urlConnectionParam != nullptr){

		return urlConnectionParam;
	}

	return nullptr;
}


bool CConnectionCollectionComp::SetUrl(const QByteArray& id, const QUrl& url) const
{
	const istd::IChangeable* objectPtr = m_collection.GetObjectPtr(id);
	if (objectPtr == nullptr){
		return false;
	}

	istd::IChangeable* notConstObjectPtr = const_cast<istd::IChangeable*>(objectPtr);
	if (notConstObjectPtr == nullptr){
		return false;
	}

	CUrlConnectionParam* urlConnectionParam =  dynamic_cast<CUrlConnectionParam*>(notConstObjectPtr);
	if (urlConnectionParam != nullptr){
		bool retVal = true;

		for (int index = 0; index < m_connectionUsageIds.GetCount(); index++){
			if (id == m_connectionUsageIds[index]){
				if (index < m_connectionUrlListCompPtr.GetCount()){
					retVal = retVal && m_connectionUrlListCompPtr[index]->SetUrl(url);
					retVal = retVal && urlConnectionParam->SetUrl(url);

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
			imtservice::IServiceConnectionParam::ConnectionType /*connectionType*/,
			const QString& /*name*/,
			const QString& /*description*/)
{
	return QByteArray();
}


int CConnectionCollectionComp::GetTracingLevel() const
{
	if (!m_tracingConfigurationCompPtr.IsValid())
	{
		return -1;
	}

	return m_tracingConfigurationCompPtr->GetTracingLevel();
}


void CConnectionCollectionComp::SetTracingLevel(int tracingLevel)
{
	if (!m_tracingConfigurationCompPtr.IsValid())
	{
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
		m_connectionUsageIds.GetCount() == m_connectionNames.GetCount() &&
		m_connectionUsageIds.GetCount() == m_connectionServiceTypeNames.GetCount() &&
		m_connectionUsageIds.GetCount() == m_connectionDescriptions.GetCount() &&
		m_connectionUsageIds.GetCount() == m_connectionTypes.GetCount() &&
		m_connectionUsageIds.GetCount() == m_connectionUrlListCompPtr.GetCount()){
		for (int index = 0; index < m_connectionUsageIds.GetCount(); index++){
			IServiceConnectionParam::ConnectionType connectionType = IServiceConnectionParam::CT_INPUT;
			if (m_connectionTypes[index] == 1){
				connectionType = IServiceConnectionParam::CT_OUTPUT;
			}
			QByteArray name = m_connectionNames[index];
			QByteArray serviceTypeName = m_connectionServiceTypeNames[index];
			QUrl url = m_connectionUrlListCompPtr[index]->GetUrl();
			QByteArray connectionUsageId = m_connectionUsageIds[index];
			QByteArray description = m_connectionDescriptions[index];

			CUrlConnectionParam urlConnectionParam(serviceTypeName, connectionUsageId, connectionType, url);
			m_collection.InsertNewObject("ConnectionInfo", name, description, &urlConnectionParam, connectionUsageId);
		}
	}
}


} // namespace agentinodata


