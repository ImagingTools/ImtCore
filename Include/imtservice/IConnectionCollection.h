#pragma once


// ACF includes
#include <ilog/ITracingConfiguration.h>

// ImtCore includes
#include <imtservice/IServiceConnectionInfo.h>
#include <imtbase/ICollectionInfo.h>
#include <imtbase/IUrlParam.h>


namespace imtservice
{


/**
	Interface for describing an service connections collection.
	\ingroup Service
*/
class IConnectionCollection:
			virtual public ilog::ITracingConfiguration
{
public:
	virtual QString GetServiceVersion() const = 0;
	virtual QString GetServiceTypeName() const = 0;
	virtual bool SetServiceTypeName(const QString& serviceTypeName) const = 0;
	virtual const imtbase::ICollectionInfo* GetUrlList() const = 0;
	virtual const QUrl* GetUrl(const QByteArray& id) const = 0;
	virtual const IServiceConnectionInfo* GetConnectionMetaInfo(const QByteArray& id) const = 0;
	virtual bool SetUrl(const QByteArray& id, const QUrl& url) const = 0;
	virtual QByteArray InsertNewConnection(
				const QByteArray& connectionId,
				const QUrl& url,
				imtservice::IServiceConnectionInfo::ConnectionType connectionType,
				const QString& name = QString(),
				const QString& description = QString() ) = 0;
};

// CUrlConnectionParam

} // namespace imtservice


