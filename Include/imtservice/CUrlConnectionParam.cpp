#include <imtservice/CUrlConnectionParam.h>



namespace imtservice
{


// public methods

CUrlConnectionParam::CUrlConnectionParam(const QByteArray& serviceName, ConnectionType connectionType, QUrl url)
{
	m_serviceName = serviceName;
	m_connectionType = connectionType;
	SetUrl(url);
}


// reimplemented (imtservice::IConnectionMetaInfo)
imtservice::IConnectionMetaInfo::ConnectionType CUrlConnectionParam::GetConnectionType() const
{
	return m_connectionType;
}


QByteArray CUrlConnectionParam::GetServiceName() const
{
	return m_serviceName;
}



} // namespace agentinodata


