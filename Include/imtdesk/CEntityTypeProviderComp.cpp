// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CEntityTypeProviderComp.h>


namespace imtdesk
{


// reimplemented (imtdesk::IEntityTypeProvider)

QString CEntityTypeProviderComp::GetEntityTypeId() const
{
	return QString::fromUtf8(*m_entityTypeIdAttrPtr);
}


QString CEntityTypeProviderComp::GetEntityTypeName() const
{
	return QString::fromUtf8(*m_entityTypeNameAttrPtr);
}


QString CEntityTypeProviderComp::GetCollectionId() const
{
	return QString::fromUtf8(*m_collectionIdAttrPtr);
}


} // namespace imtdesk
