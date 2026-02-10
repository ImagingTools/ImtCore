// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionClient.h>
#include <imtclientgql/CRemoteGqlCollectionController.h>


namespace imtbase
{
	class IRevisionController;
}


namespace imtclientgql
{


class CGqlObjectCollectionComp:
			public QObject,
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			virtual public CRemoteGqlCollectionController,
			virtual public IGqlSubscriptionClient
{
	Q_OBJECT
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGqlObjectCollectionComp);
		I_REGISTER_INTERFACE(IGqlSubscriptionClient);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_delegateCompPtr, "GqlDatabaseDelegate", "GraphQL-based document delegate for database CRUD operations", true, "GqlObjectCollectionDelegate");
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_typeNamesAttrPtr, "TypeNames", "List of type names corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", false);
		I_ASSIGN(m_clientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
		I_ASSIGN_MULTI_0(m_metaInfoCreatorListCompPtr, "MetaInfoCreators", "List of the meta-info creators corresponding to the reigstered type-IDs", false);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(const QByteArray& subscriptionId, const SubscriptionStatus& status, const QString& message) override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;

protected:
	virtual imtbase::IObjectCollection::DataPtr CreateObjectInstance(const QByteArray& typeId) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(IGqlObjectCollectionDelegate, m_delegateCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
	I_MULTITEXTATTR(m_typeNamesAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);
	I_REF(IGqlClient, m_clientCompPtr);
	I_MULTIREF(imtbase::IMetaInfoCreator, m_metaInfoCreatorListCompPtr);

	iprm::COptionsManager m_typeInfos;
};


} //namespace imtclientgql


