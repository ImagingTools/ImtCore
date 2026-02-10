// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


