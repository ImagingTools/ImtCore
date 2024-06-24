#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtbase/ITreeModelWrittable.h>


namespace imtgql
{


class CSdlCollectionControllerCompBase : public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

protected:
	virtual imtbase::CTreeItemModel* ListObjects(
		const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

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


} // namespace imtgql


