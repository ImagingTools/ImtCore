// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtserverapp/ICommandPermissionsProvider.h>


namespace imtserverapp
{


class CCommandPermissionsProviderJoinerComp:
			public icomp::CComponentBase,
			virtual public imtserverapp::ICommandPermissionsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandPermissionsProviderJoinerComp);
		I_REGISTER_INTERFACE(imtserverapp::ICommandPermissionsProvider);
		I_ASSIGN_MULTI_0(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "List of command permission providers", false);
	I_END_COMPONENT;

	// reimplemented (imtserverapp::ICommandPermissionsProvider)
	virtual LogicalCheckMode GetCommandPermissionCheckMode() const override;
	virtual QByteArrayList GetCommandIds() const override;
	virtual QByteArrayList GetCommandPermissions(const QByteArray& commandId) const override;	

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void UpdatePermissionsCache();

protected:
	I_MULTIREF(imtserverapp::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);

private:
	QMap<QByteArray, QByteArrayList> m_permissionsCache;
};


} // namespace imtbase


