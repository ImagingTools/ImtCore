// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CTaggableEntityTypeComp.h>


namespace imttag
{


// reimplemented (imtbase::IEntityTypeProvider)

QString CTaggableEntityTypeComp::GetEntityTypeId() const
{
	return m_entityTypeIdAttrPtr.IsValid() ? QString::fromUtf8(*m_entityTypeIdAttrPtr) : QString();
}


QString CTaggableEntityTypeComp::GetEntityTypeName() const
{
	return m_entityTypeNameAttrPtr.IsValid() ? *m_entityTypeNameAttrPtr : QString();
}


// reimplemented (imtbase::IObjectCollectionProvider)

const imtbase::IObjectCollection* CTaggableEntityTypeComp::GetObjectCollection() const
{
	return m_objectCollectionCompPtr.IsValid() ? m_objectCollectionCompPtr.GetPtr() : nullptr;
}


} // namespace imttag


