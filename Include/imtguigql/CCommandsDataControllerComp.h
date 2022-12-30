#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationControllerCompBase.h>


namespace imtguigql
{


class CCommandsDataControllerComp: public imtgql::CGqlRepresentationControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCommandsDataControllerComp);
		I_ASSIGN_MULTI_0(m_commandsDataProviderCompPtr, "CommandsDataProviderCompPtr", "List of commands providers", true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_commandsDataProviderCompPtr);
};


} // namespace imtguigql


