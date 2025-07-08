#pragma once


// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>

#undef SetPort

#ifdef Q_OS_WIN
#undef SetPort
#endif


namespace imtcom
{


class CServerConnectionInterfaceParam : virtual public IServerConnectionInterface
{
public:
	CServerConnectionInterfaceParam();

	void RegisterProtocol(ProtocolType protocol);

	// reimplemented (imtcom::IServerConnectionInterface)
	virtual int GetConnectionFlags() const override;
	virtual void SetConnectionFlags(int connectionFlags) override;
	virtual QString GetHost() const override;
	virtual void SetHost(const QString& host) override;
	virtual int GetPort(ProtocolType protocol) const override;
	virtual void SetPort(ProtocolType protocol, int port) override;
	virtual ProtocolTypes GetSupportedProtocols() const override;
	virtual bool GetUrl(ProtocolType protocol, QUrl& url) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	typedef QMap<ProtocolType, int> InterfaceMap;
	InterfaceMap m_interfaceMap;

	int m_connectionFlags = CF_DEFAULT;
	QString m_host;
};


} // namespace imtcom


