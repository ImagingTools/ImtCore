#pragma once


// ImtCore includes
#include <imtbase/CUrlParam.h>
#include <imtservice/IServiceConnectionParam.h>


namespace imtservice
{


class CUrlConnectionParam:
			public imtbase::CUrlParam,
			public imtservice::IServiceConnectionParam
{
public:
	typedef imtbase::CUrlParam BaseClass;

	CUrlConnectionParam();
	CUrlConnectionParam(const QByteArray& serviceName, ConnectionType connectionType, QUrl url);

	void AddExternConnection(IncomingConnectionParam IncomingConnectionParam);

	// reimplemented (imtservice::IServiceConnectionParam)
	virtual ConnectionType GetConnectionType() const override;
	virtual QByteArray GetServiceName() const override;
	virtual QList<IncomingConnectionParam> GetIncomingConnections() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	ConnectionType m_connectionType;
	QByteArray m_serviceName;
	QList<IncomingConnectionParam> m_externConnectionList;
};


} // namespace imtservice



