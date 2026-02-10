// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>


namespace imtservergql
{


class CObjectMetaInfoCollectionControllerComp:
			public CObjectCollectionControllerCompBase
{
public:
	typedef CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectMetaInfoCollectionControllerComp);
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtservergql


