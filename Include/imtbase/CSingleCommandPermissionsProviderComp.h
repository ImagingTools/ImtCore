#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/ICommandPermissionsProvider.h>


namespace imtbase
{


class CSingleCommandPermissionsProviderComp :
			public ilog::CLoggerComponentBase,
			virtual public imtbase::ICommandPermissionsProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSingleCommandPermissionsProviderComp);
		I_REGISTER_INTERFACE(imtbase::ICommandPermissionsProvider);
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


} // namespace imtbase


