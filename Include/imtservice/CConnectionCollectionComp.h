// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ilog/ITracingConfiguration.h>
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtservice/IConnectionCollection.h>
#include <imtcom/IServerConnectionInterface.h>


namespace imtservice
{


class CConnectionCollectionComp:
			public ilog::CLoggerComponentBase,
			virtual public IConnectionCollection
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CConnectionCollectionComp);
		I_REGISTER_INTERFACE(imtservice::IConnectionCollection);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info of the service", true, "ApplicationInfo");
		I_ASSIGN(m_serviceTypeId, "ServiceTypeId", "Service type ID for service", true, "");
		I_ASSIGN_MULTI_0(m_connectionNames, "ConnectionNames", "Connection names", true);
		I_ASSIGN_MULTI_0(m_connectionServiceTypeIds, "ConnectionServiceTypeIds", "Service type IDs for connections", true);
		I_ASSIGN_MULTI_0(m_connectionDescriptions, "ConnectionDescriptions", "Service connection descriptions", true);
		I_ASSIGN_MULTI_0(m_connectionTypes, "ConnectionTypes", "Service connection types: 0 - input, 1 - output", true);
		I_ASSIGN_MULTI_0(m_serverInterfaceListCompPtr, "ServerInterfaces", "List of server connection interfaces", true);
		I_ASSIGN(m_tracingConfigurationCompPtr, "TracingConfiguration", "Tracing configuration of verbose messages", true, "TracingConfiguration");
	I_END_COMPONENT;

	// reimplemented (imtservice::IConnectionCollection)
	virtual QString GetServiceVersion() const override;
	virtual QString GetServiceTypeId() const override;
	virtual bool SetServiceTypeId(const QString& serviceTypeName) const override;
	virtual const imtbase::ICollectionInfo* GetServerConnectionList() const override;
	virtual const imtcom::IServerConnectionInterface* GetServerConnection(const QByteArray& id) const override;
	virtual const IServiceConnectionInfo* GetConnectionMetaInfo(const QByteArray& id) const override;
	virtual bool SetServerConnectionInterface(const QByteArray& id, const imtcom::IServerConnectionInterface& connectionInterface) override;
	virtual QByteArray InsertNewConnection(
				const QByteArray& connectionId,
				const QUrl& url,
				IServiceConnectionInfo::ConnectionType connectionType,
				const QString& name = QString(),
				const QString& description = QString() ) override;

	// reimplemented (ilog::ITracingConfiguration)
	virtual int GetTracingLevel() const override;
	virtual void SetTracingLevel(int tracingLevel) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_ATTR(QByteArray, m_serviceTypeId);
	I_MULTIATTR(QByteArray, m_connectionNames);
	I_MULTIATTR(QByteArray, m_connectionServiceTypeIds);
	I_MULTIATTR(QByteArray, m_connectionDescriptions);
	I_MULTIATTR(int, m_connectionTypes);
	I_MULTIREF(imtcom::IServerConnectionInterface, m_serverInterfaceListCompPtr);
	I_REF(ilog::ITracingConfiguration, m_tracingConfigurationCompPtr);

	imtbase::CObjectCollection m_collection;
};


} // namespace imtservice



