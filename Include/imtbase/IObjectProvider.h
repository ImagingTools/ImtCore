#pragma once


// ACF includes
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace imtbase
{


/**
	Interface for accessing data object identfied by unique ID inside of some abstract storage.
	\ingroup Collection
*/
class IObjectProvider: virtual public IObjectCollectionInfo
{
public:
	/**
		Get the object instance with the given ID.
	*/
	virtual const istd::IChangeable* GetDataObject(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


