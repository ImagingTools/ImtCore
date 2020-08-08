#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for describing the properties of a file collection.
*/
class IFileCollectionInfo: virtual public istd::IChangeable
{
public:
	/**
		Get path to the root folder of the file collection.
	*/
	virtual QString GetCollectionRootFolder() const = 0;
};


} // namespace imtbase


