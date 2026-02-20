// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/ITracingConfiguration.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtcom/IServerConnectionInterface.h>
#include <imtservice/IServiceConnectionInfo.h>


namespace imtservice
{


/**
	Interface for describing an service connections collection.
	\ingroup Service
*/
class IConnectionCollection: virtual public ilog::ITracingConfiguration
{
public:
	virtual QString GetServiceVersion() const = 0;
	virtual QString GetServiceTypeId() const = 0;
	virtual bool SetServiceTypeId(const QString& serviceTypeName) const = 0;
	virtual const imtbase::ICollectionInfo* GetServerConnectionList() const = 0;
	virtual const imtcom::IServerConnectionInterface* GetServerConnection(const QByteArray& id) const = 0;
	virtual const IServiceConnectionInfo* GetConnectionMetaInfo(const QByteArray& id) const = 0;
	virtual bool SetServerConnectionInterface(const QByteArray& id, const imtcom::IServerConnectionInterface& connectionInterface) = 0;
	virtual QByteArray InsertNewConnection(
				const QByteArray& connectionId,
				const QUrl& url,
				IServiceConnectionInfo::ConnectionType connectionType,
				const QString& name = QString(),
				const QString& description = QString() ) = 0;
};


typedef istd::TUniqueInterfacePtr<IConnectionCollection> IConnectionCollectionUniquePtr;
typedef istd::TSharedInterfacePtr<IConnectionCollection> IConnectionCollectionSharedPtr;


} // namespace imtservice


