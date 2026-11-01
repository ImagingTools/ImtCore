// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/CNotificationTypeListComp.h>


namespace imtnotify
{


// reimplemented (imtnotify::INotificationTypeList)

int CNotificationTypeListComp::GetTypesCount() const
{
	return m_typesCompPtr.GetCount();
}


const INotificationTypeInfo* CNotificationTypeListComp::GetTypeInfo(int index) const
{
	return (index >= 0 && index < m_typesCompPtr.GetCount()) ? m_typesCompPtr[index] : nullptr;
}


int CNotificationTypeListComp::GetObjectTypesCount() const
{
	return m_objectTypesCompPtr.GetCount();
}


const INotificationTypeInfo* CNotificationTypeListComp::GetObjectTypeInfo(int index) const
{
	return (index >= 0 && index < m_objectTypesCompPtr.GetCount()) ? m_objectTypesCompPtr[index] : nullptr;
}


} // namespace imtnotify


