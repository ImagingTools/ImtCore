// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/IProductServerRequestPolicy.h>


namespace imtservergql
{


/**
	The requests of the servers of products, declared by command or by collection.
	A command declared by name takes its permission. A request of a declared collection takes the permission to read
	one of its objects, to view it, or to edit it for a mutation that is not declared as reading.
*/
class CProductServerRequestPolicyComp:
			public icomp::CComponentBase,
			virtual public IProductServerRequestPolicy
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductServerRequestPolicyComp);
		I_REGISTER_INTERFACE(IProductServerRequestPolicy);
		I_ASSIGN_MULTI_0(m_openCommandIdsAttrPtr, "OpenCommandIds", "Commands any connected server may send", false);
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "Commands declared by name", false);
		I_ASSIGN_MULTI_0(m_commandPermissionsAttrPtr, "CommandPermissions", "Permission of each command, in the order of CommandIds", false);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collections whose requests are declared", false);
		I_ASSIGN_MULTI_0(m_collectionViewPermissionsAttrPtr, "CollectionViewPermissions", "Permission to view each collection, in the order of CollectionIds", false);
		I_ASSIGN_MULTI_0(m_collectionEditPermissionsAttrPtr, "CollectionEditPermissions", "Permission to edit each collection, in the order of CollectionIds", false);
		I_ASSIGN_MULTI_0(m_viewCommandIdsAttrPtr, "ViewCommandIds", "Mutations of a collection that only read it, e.g. opening a document", false);
		I_ASSIGN_MULTI_0(m_objectReadCommandIdsAttrPtr, "ObjectReadCommandIds", "Commands of a collection reading one object of it", false);
		I_ASSIGN_MULTI_0(m_collectionObjectReadPermissionsAttrPtr, "CollectionObjectReadPermissions", "Permission to read one object of each collection, in the order of CollectionIds", false);
	I_END_COMPONENT;

	// reimplemented (IProductServerRequestPolicy)
	virtual bool FindRequiredPermission(const imtgql::CGqlRequest& gqlRequest, QByteArray& permission) const override;

private:
	I_MULTIATTR(QByteArray, m_openCommandIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_commandPermissionsAttrPtr);
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_collectionViewPermissionsAttrPtr);
	I_MULTIATTR(QByteArray, m_collectionEditPermissionsAttrPtr);
	I_MULTIATTR(QByteArray, m_viewCommandIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_objectReadCommandIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_collectionObjectReadPermissionsAttrPtr);
};


} // namespace imtservergql
