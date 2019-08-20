#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace imtbase
{


/**
	Interface for access of generally defined list of data objects.
*/
class IObjectProvider: virtual public IObjectCollectionInfo
{
public:
	typedef istd::TSmartPtr<istd::IChangeable> ObjectPtr;

	/**
		Get the object instance with the given ID.
	*/
	virtual const istd::IChangeable* GetDataObject(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


