#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtgql/ICommandPermissionsProvider.h>


namespace imtgql
{


class CPermissionsControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsControllerComp);
		I_ASSIGN_MULTI_0(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "List of command permission providers", true);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CHierarchicalItemModelPtr CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_MULTIREF(imtgql::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
};


} // namespace imtgql


