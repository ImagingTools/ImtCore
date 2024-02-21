#pragma once


// ImtCore includes
#include <imtbase/CUrlParam.h>
#include <imtservice/IConnectionMetaInfo.h>


namespace imtservice
{


class CUrlConnectionParam:
			public imtbase::CUrlParam,
			public imtservice::IConnectionMetaInfo
{
public:
	CUrlConnectionParam(const QByteArray& serviceName, ConnectionType connectionType, QUrl url);

	// reimplemented (imtservice::IConnectionMetaInfo)
	virtual ConnectionType GetConnectionType() const override;
	virtual QByteArray GetServiceName() const override;

private:
	ConnectionType m_connectionType;
	QByteArray m_serviceName;
};


} // namespace imtservice



