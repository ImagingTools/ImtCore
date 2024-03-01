#pragma once


// ImtCore includes
#include <imtservice/IServiceConnectionLinkParam.h>
#include <imtservice/IConnectionStatus.h>


namespace imtservice
{


class CUrlConnectionLinkParam:
			public virtual imtservice::IServiceConnectionLinkParam,
			public virtual imtservice::IConnectionStatus
{
public:
	CUrlConnectionLinkParam();
	CUrlConnectionLinkParam(const QByteArray& serviceTypeName, const QByteArray& usageId, const QByteArray& dependantServiceConnectionId);

	// reimplemented (imtservice::IServiceInfo)
	virtual ConnectionType GetConnectionType() const override;
	virtual QByteArray GetServiceTypeName() const override;
	virtual QByteArray GetUsageId() const override;

	// reimplemented (imtservice::IServiceConnectionLinkParam)
	virtual QByteArray GetDependantServiceConnectionId() const override;

	// reimplemented (imtservice::IConnectionStatus)
	virtual ConnectionStatus GetConnectionStatus() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_serviceTypeName;
	QByteArray m_usageId;
	QByteArray m_dependantServiceConnectionId;
	ConnectionStatus m_connectionStatus;
};


} // namespace imtservice



