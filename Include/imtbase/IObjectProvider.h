#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IElementList.h>


namespace imtbase
{


/**
	Interface for access of general defined list of data objects.
*/
class IObjectProvider: virtual public IElementList
{
public:
	typedef istd::TSmartPtr<istd::IChangeable> ObjectPtr;

	/**
		Get the list of supported object types. Only elements of supported types can be added to the container.
	*/
	virtual const iprm::IOptionsList* GetSupportedObjectTypes() const = 0;

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


