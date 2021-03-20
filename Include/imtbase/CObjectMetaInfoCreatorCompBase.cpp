#pragma once


// ImtCore includes
#include <imtbase/CObjectMetaInfoCreatorCompBase.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

IMetaInfoCreator::TypeIds CObjectMetaInfoCreatorCompBase::GetSupportedTypeIds() const
{
	TypeIds retVal;

	retVal.push_back(*m_objectTypeIdAttrPtr);
	
	return retVal;
}


} // namespace imtlic


