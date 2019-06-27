#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IElementList.h>


namespace imtbase
{


/**
	Interface for access of generally defined list of data objects.
*/
class IObjectProvider: virtual public IElementList
{
public:
	typedef istd::TSmartPtr<istd::IChangeable> ObjectPtr;

	/**
		Get the list of the available object types.
	*/
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const = 0;

	/**
		Get the type-ID of the object.
	*/
	virtual Id GetObjectTypeId(const QByteArray& objectId) const = 0;

	/**
		Get the object instance with the given ID.
	*/
	virtual const istd::IChangeable* GetDataObject(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


