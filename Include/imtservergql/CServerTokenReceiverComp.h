// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtservergql/IServerTokenReceiver.h>


namespace imtservergql
{


/**
	The requests of the servers of products, declared by command or by collection.
	A command takes the permission written with it, "Command=Permission"; "Command=" opens it to any connected server.
	A collection is written "Collection=View", "Collection=View,Edit" or "Collection=View,Edit,ObjectRead"; without an
	edit permission its mutations are closed. Only the commands of the
	generic collection protocol are resolved by their collection: reading one object takes the object-read permission
	(the view permission without one), a query and opening a document the view permission, a mutation the edit one.
*/
class CServerTokenReceiverComp:
			public ilog::CLoggerComponentBase,
			virtual public IServerTokenReceiver
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CServerTokenReceiverComp);
		I_REGISTER_INTERFACE(IServerTokenReceiver);
		I_ASSIGN(m_permissionTreeCompPtr, "PermissionTree", "Product of this server", true, "PermissionTree");
		I_ASSIGN(m_tokenManagerCompPtr, "TokenManager", "Where the tokens issued for other servers are kept", true, "TokenManager");
		I_ASSIGN_MULTI_0(m_commandPermissionsAttrPtr, "CommandPermissions", "Commands the servers of products may send, each as 'Command=Permission'; 'Command=' opens it to any connected server", false);
		I_ASSIGN_MULTI_0(m_collectionPermissionsAttrPtr, "CollectionPermissions", "Collections the servers of products may use, each as 'Collection=View[,Edit[,ObjectRead]]'", false);
	I_END_COMPONENT;

	// reimplemented (IServerTokenReceiver)
	virtual QByteArray GetServerProductId() const override;
	virtual imtauth::IPersonalAccessTokenManager* GetTokenManager() const override;
	virtual bool FindRequiredPermission(const imtgql::CGqlRequest& gqlRequest, QByteArray& permission) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	struct CollectionPermissions
	{
		QByteArray view;
		QByteArray edit;
		QByteArray objectRead;
	};

private:
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_MULTIATTR(QByteArray, m_commandPermissionsAttrPtr);
	I_MULTIATTR(QByteArray, m_collectionPermissionsAttrPtr);

	QHash<QByteArray, QByteArray> m_permissionsOfCommands;
	QHash<QByteArray, CollectionPermissions> m_permissionsOfCollections;
};


} // namespace imtservergql
