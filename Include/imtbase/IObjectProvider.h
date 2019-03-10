#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtbase
{


/**
	Interface for access of general defined list of data objects.
*/
class IObjectProvider: virtual public istd::IChangeable
{
public:
	typedef QList<QByteArray> Ids;

	/**
		Get the list with basicaly informations about stored objects.
	*/
	virtual const iprm::IOptionsList& GetObjectInfoList() const = 0;

	/**
		Get the object instance with the given ID.
	*/
	virtual const istd::IChangeable* GetObject(const QByteArray& objectId) const = 0;

	/**
		Get the type-ID of an object with the given ID.
	*/
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


