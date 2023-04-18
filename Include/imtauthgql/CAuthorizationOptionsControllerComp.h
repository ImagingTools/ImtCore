#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtauthgql
{


class CAuthorizationOptionsControllerComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef imtgql::CGqlRepresentationDataControllerComp BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationOptionsControllerComp);
		I_ASSIGN(m_superuserProviderCompPtr, "SuperuserProvider", "Superuser provider", false, "SuperuserProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtauth::ISuperuserProvider, m_superuserProviderCompPtr);
};


} // namespace imtauthgql


