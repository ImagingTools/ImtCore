#pragma once


// ImtCore includes
#include <imtbase/CUrlParam.h>
#include <imtservice/IServiceConnectionInfo.h>
#include <imtservice/IConnectionStatus.h>


namespace imtservice
{


class CServiceConnectionInfo:
			public imtbase::CUrlParam,
			virtual public IServiceConnectionInfo,
			virtual public imtservice::IConnectionStatus
{
public:
	typedef imtbase::CUrlParam BaseClass;
	
	CServiceConnectionInfo();
	
	void SetConnectionType(ConnectionType connectionType);
	void SetServiceTypeName(const QByteArray& serviceTypeName);
	void SetUsageId(const QByteArray& usageId);
	void SetDefaultUrl(const QUrl& url);
	
	// reimplemented (imtservice::IServiceConnectionInfo)
	virtual ConnectionType GetConnectionType() const override;
	virtual QByteArray GetServiceTypeName() const override;
	virtual QByteArray GetUsageId() const override;
	virtual QUrl GetDefaultUrl() const override;
	
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
	QUrl m_defaultUrl;
	ConnectionStatus m_connectionStatus;
};


} // namespace imtservice



