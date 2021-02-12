#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrepo
{


/**
	Interface for describing the properties of a file collection.
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

	virtual FileCollectionLayout GetCollectionFileLayout() const = 0;

	/**
		Get path to the root folder of the file collection.
	*/
	virtual QString GetCollectionRootFolder() const = 0;
};


} // namespace imtrepo


