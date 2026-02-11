// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CGqlControllerComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtservergql


