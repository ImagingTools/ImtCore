// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Something what can be identified inside of a global system.
*/
class IIdentifiable: virtual public istd::IChangeable
{
public:
	/**
		Get the global unique identifier of the object.
	*/
	virtual QByteArray GetObjectUuid() const = 0;

	/**
		Set the global unique identifier of the object.
	*/
	virtual bool SetObjectUuid(const QByteArray& identifier) = 0;
};


inline QByteArray GetObjectUuid(const istd::IChangeable* objectPtr)
{
	const IIdentifiable* identifiablePtr = dynamic_cast<const IIdentifiable*>(objectPtr);

	return identifiablePtr == nullptr ? QByteArray() : identifiablePtr->GetObjectUuid();
}


inline bool SetObjectUuid(istd::IChangeable* objectPtr, const QByteArray& identifier)
{
	IIdentifiable* identifiablePtr = dynamic_cast<IIdentifiable*>(objectPtr);

	return identifiablePtr == nullptr ? false : identifiablePtr->SetObjectUuid(identifier);
}


} // namespace imtbase


