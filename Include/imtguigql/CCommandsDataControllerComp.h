#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtguigql
{


class CCommandsDataControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCommandsDataControllerComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN_MULTI_0(m_commandsDataProviderCompPtr, "CommandsDataProviderCompPtr", "List of commands providers", true);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_MULTIREF(imtbase::IItemBasedRepresentationDataProvider, m_commandsDataProviderCompPtr);
};


} // namespace imtguigql


