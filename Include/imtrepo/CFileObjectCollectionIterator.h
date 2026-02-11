// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtrepo/IFileObjectCollection.h>


namespace imtrepo
{


/**
	Common interface for a object collection query.
	\ingroup Collection
*/
class CFileObjectCollectionIterator: virtual public imtbase::IObjectCollectionIterator
{
	typedef imtbase::IObjectCollection::DataPtr DataPtr;
public:
	CFileObjectCollectionIterator(IFileObjectCollection* fileObjectCollectionPtr);

	// reimplemented (imtbase::IObjectCollectionIterator)
	virtual bool Next() const override;
	virtual bool Previous() const override;
	virtual QByteArray GetObjectId() const override;
	virtual QByteArray GetObjectTypeId() const override;
	virtual bool GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo() const override;
	virtual idoc::MetaInfoPtr GetCollectionMetaInfo() const override;
	virtual QVariant GetElementInfo(int infoType) const override;
	virtual QVariant GetElementInfo(QByteArray infoId) const override;
private:

	IFileObjectCollection* m_fileObjectCollectionPtr;

	mutable int m_currentIndex;
};


} // namespace imtrepo


