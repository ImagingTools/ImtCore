// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
// ImtCore includes
#include <imtbase/CObjectMetaInfoCreatorCompBase.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

IMetaInfoCreator::TypeIds CObjectMetaInfoCreatorCompBase::GetSupportedTypeIds() const
{
	TypeIds retVal;

	if (!m_objectTypeIdsAttrPtr.IsValid()){
		return retVal;
	}

	int count = m_objectTypeIdsAttrPtr.GetCount();
	for (int i = 0; i < count; i++){
		retVal.push_back(m_objectTypeIdsAttrPtr[i]);
	}
	
	return retVal;
}


} // namespace imtlic


