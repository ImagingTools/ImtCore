#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>
#include <ifile/IFileResourceTypeConstraints.h>


namespace imtrepo
{


class IFileCollectionItem;


/**
	Interface for describing the structure of a file-based collection.
	\ingroup FileRepository
*/
class IFileCollectionInfo: virtual public istd::IChangeable
{
public:
	/**
		Type of the repository layout.
	*/
	enum FileCollectionLayout
	{
		/**
			All repository files are placed in the same folder.
		*/
		FCL_FLAT,

		/**
			Each repository is placed in its own folder.
		*/
		FCL_FOLDER
	};

	/**
		Get type contraints describing each supported type of a resource.
		\return	Type constraints, if available or \c nullptr, if no type constraints were defined.
	*/
	virtual const ifile::IFileResourceTypeConstraints* GetFileTypeConstraints() const = 0;

	/**
		Get file information related to a object
		\param objectId	ID of the file in the collection
		\return	Related informations \sa FileInfo
	*/
	virtual bool GetFileInfo(const QByteArray& objectId, IFileCollectionItem& collectionItem) const = 0;

	/**
		Get information about a type of the file collection layout.
	*/
	virtual FileCollectionLayout GetCollectionFileLayout() const = 0;

	/**
		Get path to the root folder of the file collection.
	*/
	virtual QString GetCollectionRootFolder() const = 0;

	/**
		Get repository revision.
	*/
	virtual int GetRepositoryRevision() const = 0;
};


} // namespace imtrepo


