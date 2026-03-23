// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// Qt includes
#include <QHash>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IRoleInfoProvider.h>


namespace imtauth
{


class CRoleCollectionAdapterComp:
			public icomp::CComponentBase,
			virtual public IRoleInfoProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionAdapterComp);
		I_REGISTER_INTERFACE(IRoleInfoProvider);
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Adapting the collection to the provider", true, "RoleCollection");
	I_END_COMPONENT;

	// reimplemented (IRoleInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRoleList() const override;
	virtual imtauth::IRoleUniquePtr GetRole(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);

	// In-memory cache to avoid repeated DB queries for the same role.
	// Roles are shared across many users, so caching avoids N*M individual lookups.
	mutable QHash<QByteArray, imtbase::IObjectCollection::DataPtr> m_roleCache;
};


} // namespace imtauth


