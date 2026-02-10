// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcom/CServerConnectionInterfaceParam.h>
#include <imtservice/IServiceConnectionInfo.h>
#include <imtservice/IConnectionStatus.h>


namespace imtservice
{


class CServiceConnectionInfo:
			public imtcom::CServerConnectionInterfaceParam,
			virtual public IServiceConnectionInfo,
			virtual public IConnectionStatus
{
public:
	typedef imtcom::CServerConnectionInterfaceParam BaseClass;

	CServiceConnectionInfo();
	
	void SetConnectionType(ConnectionType connectionType);
	void SetServiceTypeId(const QByteArray& serviceTypeId);
	void SetDefaultServiceInterface(const imtcom::IServerConnectionInterface& defaultInterface);
	
	// reimplemented (IServiceConnectionInfo)
	virtual ConnectionType GetConnectionType() const override;
	virtual QByteArray GetServiceTypeId() const override;
	virtual const imtcom::IServerConnectionInterface& GetDefaultInterface() const override;

	// reimplemented (IConnectionStatus)
	virtual ConnectionStatus GetConnectionStatus() const override;
	
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;
	
	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
	
protected:
	ConnectionType m_connectionType;
	QByteArray m_serviceTypeId;
	imtcom::CServerConnectionInterfaceParam m_defaultConnection;
	ConnectionStatus m_connectionStatus;
};


} // namespace imtservice


