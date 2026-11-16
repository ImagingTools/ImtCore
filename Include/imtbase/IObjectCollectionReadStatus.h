// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Optional collection interface for distinguishing a confirmed access refusal from a failed read.
	No last-error state is kept: the status belongs to this invocation only.
*/
class IObjectCollectionReadStatus: virtual public istd::IPolymorphic
{
public:
	/**
		Read an object with the same success and data semantics as IObjectCollection::GetObjectData.
		Set \a isAccessDenied to false on entry and to true only for a confirmed access refusal.
		A refused read returns false; other failures leave isAccessDenied false.
	*/
	virtual bool GetObjectDataWithStatus(
				const IObjectCollection::Id& objectId,
				IObjectCollection::DataPtr& dataPtr,
				bool& isAccessDenied,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const = 0;
};


} // namespace imtbase
