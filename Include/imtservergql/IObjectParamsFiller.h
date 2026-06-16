// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>


namespace imtservergql
{


/**
	Interface for filling a params set for an object identified by ID.

	Implementations populate iprm::IParamsSet with metadata specific
	to the given object. The caller is responsible for converting
	the output into the appropriate SDL representation for serialization.
*/
class IObjectParamsFiller: virtual public istd::IPolymorphic
{
public:
	/**
		Fill parameters for the given object.
		@param objectId ID of the object.
		@param paramsSet Output params set to populate.
		@param contextTenantId ID of the current tenant (from GQL context).
		@return true if parameters were successfully provided.
	*/
	virtual bool FillParams(
				const QByteArray& objectId,
				iprm::IParamsSet& paramsSet,
				const QByteArray& contextTenantId = QByteArray()) const = 0;
};


} // namespace imtservergql
