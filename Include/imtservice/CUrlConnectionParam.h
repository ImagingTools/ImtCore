#pragma once


// ImtCore includes
#include <imtbase/CUrlParam.h>
#include <imtservice/IServiceConnectionParam.h>
#include <imtservice/IConnectionStatus.h>


namespace imtservice
{


class CUrlConnectionParam:
			public imtbase::CUrlParam,
			public imtservice::IServiceConnectionParam,
			public imtservice::IConnectionStatus
{
public:
	typedef imtbase::CUrlParam BaseClass;

	CUrlConnectionParam();
	CUrlConnectionParam(const QByteArray& serviceTypeName, const QByteArray& usageId, ConnectionType connectionType, QUrl url);

	void AddExternConnection(IncomingConnectionParam IncomingConnectionParam);
	void SetConnectionType(ConnectionType connectionType);
	void SetServiceTypeName(const QByteArray& serviceTypeName);
	void SetUsageId(const QByteArray& usageId);

	// reimplemented (imtservice::IServiceInfo)
	virtual ConnectionType GetConnectionType() const override;
	virtual QByteArray GetServiceTypeName() const override;
	virtual QByteArray GetUsageId() const override;
	virtual QUrl GetDefaultUrl() const override;

	// reimplemented (imtservice::IServiceConnectionParam)
	virtual QList<IncomingConnectionParam> GetIncomingConnections() const override;

	// reimplemented (imtservice::IConnectionStatus)
	virtual ConnectionStatus GetConnectionStatus() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	ConnectionType m_connectionType;
	QByteArray m_serviceTypeName;
	QByteArray m_usageId;
	QList<IncomingConnectionParam> m_externConnectionList;
	ConnectionStatus m_connectionStatus;
	QUrl m_defaultUrl;
};


} // namespace imtservice



