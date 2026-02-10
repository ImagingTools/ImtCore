// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtbase/ITreeModelWrittable.h>


namespace imtservergql
{


class CSdlCollectionControllerCompBase : public CObjectCollectionControllerCompBase
{
public:
	typedef CObjectCollectionControllerCompBase BaseClass;
	I_BEGIN_BASE_COMPONENT(CSdlCollectionControllerCompBase)
	I_END_COMPONENT

protected:
	virtual imtbase::CTreeItemModel* ListObjects(
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	virtual imtbase::ITreeModelWrittable* CreateSdlItem(
				const imtgql::CGqlRequest& gqlRequest,
				const QByteArray& objectId,
				const imtbase::IObjectCollection::DataPtr& dataPtr,
				QString& errorMessage) const = 0;

	virtual imtbase::ITreeModelWrittable* CreateSdlItemList(
				const imtgql::CGqlRequest& gqlRequest,
				int pagesCount,
				int elementsCount,
				const imtbase::IObjectCollectionIterator* objectCollectionIteratorPtr,
				QString& errorMessage) const = 0;

};


} // namespace imtservergql


