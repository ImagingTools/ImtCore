/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


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


