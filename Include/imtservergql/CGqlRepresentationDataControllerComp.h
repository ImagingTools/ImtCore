// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/IRepresentationController.h>
#include <imtgql/IGqlRepresentationController.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace imtservergql
{


class CGqlRepresentationDataControllerComp:
			virtual public imtgql::IGqlRepresentationController,
			public imtservergql::CPermissibleGqlRequestHandlerComp
{
public:
	typedef imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationDataControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRepresentationController);
		I_ASSIGN(m_dataModelCompPtr, "DataModel", "Data model used for representation", true, "DataModel");
		I_ASSIGN(m_representationControllerCompPtr, "RepresentationController", "Controller for data model representation", true, "RepresentationController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlRepresentationController)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual iprm::IParamsSetUniquePtr CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_REF(istd::IChangeable, m_dataModelCompPtr);
	I_REF(imtserverapp::IRepresentationController, m_representationControllerCompPtr);
};


} // namespace imtservergql


