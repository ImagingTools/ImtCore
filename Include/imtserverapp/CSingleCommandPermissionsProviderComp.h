// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtserverapp/ICommandPermissionsProvider.h>


namespace imtserverapp
{


class CSingleCommandPermissionsProviderComp :
			public ilog::CLoggerComponentBase,
			virtual public ICommandPermissionsProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSingleCommandPermissionsProviderComp);
		I_REGISTER_INTERFACE(ICommandPermissionsProvider);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "ID of the command permission are requested for", true, "");
		I_ASSIGN_MULTI_0(m_commandPermissionsAttrPtr, "CommandPermissions", "List of command permissons", true);
	I_END_COMPONENT;

	// reimplemented (imtgql::ICommandPermissionsProvider)
	virtual LogicalCheckMode GetCommandPermissionCheckMode() const override;
	virtual QByteArrayList GetCommandIds() const override;
	virtual QByteArrayList GetCommandPermissions(const QByteArray& commandId) const override;

protected:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_MULTIATTR(QByteArray, m_commandPermissionsAttrPtr);
};


} // namespace imtserverapp


