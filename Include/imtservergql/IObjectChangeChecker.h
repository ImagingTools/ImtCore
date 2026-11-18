// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


/**
	The rules of a collection for changing an object, for those who change it past the controller of the collection.
*/
class IObjectChangeChecker: virtual public istd::IPolymorphic
{
public:
	/**
		Check that the sender of the request may change the object.
		\param dataPtr The new data of the object; null when only the access to the object is in question.
	*/
	virtual bool CheckObjectChange(
				const imtgql::CGqlRequest& gqlRequest,
				const QByteArray& objectId,
				const istd::IChangeable* dataPtr,
				QString& errorMessage) const = 0;
};


} // namespace imtservergql
