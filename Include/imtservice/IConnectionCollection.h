#pragma once

// ImtCore includes
#include <imtservice/IServiceConnectionParam.h>
#include <imtbase/ICollectionInfo.h>
#include <imtbase/IUrlParam.h>


namespace imtservice
{


/**
	Interface for describing an service connections collection.
	\ingroup Service
*/
class IConnectionCollection: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo* GetUrlList() const = 0;
	virtual const QUrl* GetUrl(const QByteArray& id) const = 0;
	virtual const IServiceConnectionParam* GetConnectionMetaInfo(const QByteArray& id) const = 0;
	virtual bool SetUrl(const QByteArray& id, const QUrl& url) const = 0;
	virtual QByteArray InsertNewConnection(
				const QByteArray& connectionId,
				const QUrl& url,
				imtservice::IServiceConnectionParam::ConnectionType connectionType,
				const QString& name = QString(),
				const QString& description = QString() ) = 0;
	virtual QString GetServiceTypeName() const = 0;
};

// CUrlConnectionParam

} // namespace imtservice


