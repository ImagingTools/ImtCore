#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/ICommandPermissionsProvider.h>


namespace imtgql
{


class CCommandPermissionsProviderJoinerComp:
			public icomp::CComponentBase,
			virtual public imtgql::ICommandPermissionsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandPermissionsProviderJoinerComp);
		I_REGISTER_INTERFACE(imtgql::ICommandPermissionsProvider);
		I_ASSIGN_MULTI_0(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "List of command permission providers", true);
	I_END_COMPONENT;

	// reimplemented (imtgql::ICommandPermissionsProvider)
	virtual LogicalCheckMode GetCommandPermissionCheckMode() const override;
	virtual QByteArrayList GetCommandIds() const override;
	virtual QByteArrayList GetCommandPermissions(const QByteArray& commandId) const override;	

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void UpdatePermissionsCache();

protected:
	I_MULTIREF(imtgql::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);

private:
	QMap<QByteArray, QByteArrayList> m_permissionsCache;
};


} // namespace imtgql


