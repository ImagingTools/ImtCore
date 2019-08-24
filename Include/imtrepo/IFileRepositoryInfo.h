#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrepo
{


/**
	Interface for describing the properties of a file repository.
*/
class IFileRepositoryInfo: virtual public istd::IChangeable
{
public:
	/**
		Get location of the file repository.
		\return Path to the repository location or an empty string if no central location exists.
	*/
	virtual QString GetRepositoryPath() const = 0;
};


} // namespace imtrepo


