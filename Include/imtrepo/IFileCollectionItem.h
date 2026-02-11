// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtrepo
{


/**
	Interface for describing the single item in the file-based collection.
*/
class IFileCollectionItem: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetFileId() const = 0;
	virtual QString GetFilePath() const = 0;
	virtual QString GetName() const = 0;
	virtual QByteArray GetTypeId() const = 0;
	virtual const idoc::IDocumentMetaInfo& GetCollectionMetaInfo() const = 0;
	virtual idoc::MetaInfoPtr GetContentsMetaInfo() const = 0;
	virtual int GetRepositoryRevision() const = 0;
};


} // namespace imtrepo


