// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CCollectionObjectProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionObjectProviderComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtservergql


