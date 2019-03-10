#pragma once


// ACF includes
#include <imtbase/IObjectProvider.h>


namespace imtbase
{


/**
	Iterface for an object container.
*/
class IObjectContainer: virtual public IObjectProvider
{
public:
	/**
		Create a new object instance in the container.
		\param typeId			Type-ID of the object should be created.
		\param name				Human readable name of the object.
		\param description		Description for the object.
		\return UUID of the created object, if the operation was successful or an empty ID otherwise.
	*/
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description) = 0;

	/**
		Remove an existing object from the container.
		\param objectId ID of the object should be removed.
		\return \c true, if the operation was successful or \false otherwise.
		\note The method should return \c false only if an object could not be removed and remains in the container!
	*/
	virtual bool RemoveObject(const QByteArray& objectId) = 0;
};


} // namespace imtbase


