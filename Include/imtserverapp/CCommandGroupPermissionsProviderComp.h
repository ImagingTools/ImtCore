// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtserverapp/ICommandPermissionsProvider.h>


namespace imtserverapp
{


/**
	Commands that need the same permissions. With a collection, the commands are declared as "Collection/Command".
	A group without permissions declares no command; an empty permission opens its commands.
*/
class CCommandGroupPermissionsProviderComp:
			public icomp::CComponentBase,
			virtual public ICommandPermissionsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandGroupPermissionsProviderComp);
		I_REGISTER_INTERFACE(ICommandPermissionsProvider);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection the commands are declared for", false, "");
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "Commands of the group", true);
		I_ASSIGN_MULTI_0(m_commandPermissionsAttrPtr, "CommandPermissions", "Permissions each command of the group needs", false);
	I_END_COMPONENT;

	// reimplemented (ICommandPermissionsProvider)
	virtual LogicalCheckMode GetCommandPermissionCheckMode() const override;
	virtual QByteArrayList GetCommandIds() const override;
	virtual QByteArrayList GetCommandPermissions(const QByteArray& commandId) const override;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_commandPermissionsAttrPtr);
};


} // namespace imtserverapp
