#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtqml
{


class CCommandPermissionsFilterComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandPermissionsFilterComp);
		I_REGISTER_INTERFACE(imtgql::CGqlRepresentationDataControllerComp);
		I_ASSIGN(m_commandsProviderCompPtr, "CommandsDataProvider", "Commands Data Provider", true, "CommandsDataProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlRequestHandler, m_commandsProviderCompPtr);
};


} // namespace imtqml


