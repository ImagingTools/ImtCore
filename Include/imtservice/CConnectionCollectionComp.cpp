#include <imtservice/CConnectionCollectionComp.h>


namespace imtservice
{


// public methods

// reimplemented (imtservice::IConnectionCollection)

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
	CUrlConnectionParam* urlConnectionParam =  dynamic_cast<CUrlConnectionParam*>(const_cast<istd::IChangeable*>(m_collection.GetObjectPtr(id)));
	if (urlConnectionParam != nullptr){
		for (int index = 0; index < m_connectionIds.GetCount(); index++){
			if (id == m_connectionIds[index]){
				if (index < m_connectionUrlListCompPtr.GetCount()){
					m_connectionUrlListCompPtr[index]->SetUrl(url);
					urlConnectionParam->SetUrl(url);

					break;
				}
			}
		}

		return true;
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


// reimplemented (icomp::CComponentBase)

void CConnectionCollectionComp::OnComponentCreated()
{
	if (
		m_connectionIds.GetCount() == m_serviceNames.GetCount() &&
		m_connectionIds.GetCount() == m_connectionDescriptions.GetCount() &&
		m_connectionIds.GetCount() == m_connectionTypes.GetCount() &&
		m_connectionIds.GetCount() == m_connectionUrlListCompPtr.GetCount()){
		for (int index = 0; index < m_connectionIds.GetCount(); index++){
			IServiceConnectionParam::ConnectionType connectionType = IServiceConnectionParam::CT_INPUT;
			if (m_connectionTypes[index] == 1){
				connectionType = IServiceConnectionParam::CT_OUTPUT;
			}
			QByteArray serviceName = m_serviceNames[index];
			QUrl url = m_connectionUrlListCompPtr[index]->GetUrl();
			CUrlConnectionParam urlConnectionParam(serviceName, connectionType, url);
			QByteArray connectionId = m_connectionIds[index];
			QByteArray description = m_connectionDescriptions[index];
			m_collection.InsertNewObject("Connections", connectionId, description, &urlConnectionParam, connectionId);
		}
	}
}


} // namespace agentinodata


