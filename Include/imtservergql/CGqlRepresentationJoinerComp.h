// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlRepresentationDataControllerCompBase.h>


namespace imtservergql
{


class CGqlRepresentationJoinerComp: public CGqlRepresentationControllerCompBase
{
public:
	typedef CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationJoinerComp);
		I_ASSIGN_MULTI_0(m_representationControllersCompPtr, "RepresentsationControllers", "List of representation controllers", true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

protected:
	I_MULTIREF(imtgql::IGqlRepresentationController, m_representationControllersCompPtr);
};


} // namespace imtservergql


