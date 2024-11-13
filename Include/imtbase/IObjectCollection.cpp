#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{

const QByteArray IObjectCollection::CN_OBJECT_DATA_CHANGED = QByteArrayLiteral("imtbase:IObjectCollection::ObjectDataChanged");


IHierarchicalStructure* IObjectCollection::GetCollectionStructure() const
{
	return nullptr;
}


} // namespace imtbase


