#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ICommandPermissionsProvider.h>


namespace imtbase
{


class CCommandPermissionsProviderJoinerComp:
			public icomp::CComponentBase,
			virtual public imtbase::ICommandPermissionsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandPermissionsProviderJoinerComp);
		I_REGISTER_INTERFACE(imtbase::ICommandPermissionsProvider);
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
	I_MULTIREF(imtbase::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);

private:
	QMap<QByteArray, QByteArrayList> m_permissionsCache;
};


} // namespace imtbase


