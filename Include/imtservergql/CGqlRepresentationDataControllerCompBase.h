// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlRepresentationController.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CGqlRepresentationControllerCompBase: virtual public imtgql::IGqlRepresentationController, public CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationControllerCompBase);
		I_REGISTER_INTERFACE(IGqlRepresentationController);
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtservergql


