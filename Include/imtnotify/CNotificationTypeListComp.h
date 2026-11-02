// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtnotify/INotificationTypeList.h>


namespace imtnotify
{


/**
	The notification types of a module and the types of their objects, each one a CNotificationTypeComp element.
*/
class CNotificationTypeListComp:
			public icomp::CComponentBase,
			virtual public INotificationTypeList
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationTypeListComp);
		I_REGISTER_INTERFACE(INotificationTypeList);
		I_ASSIGN_MULTI_0(m_typesCompPtr, "Types", "Notification types of the module", false);
		I_ASSIGN_MULTI_0(m_objectTypesCompPtr, "ObjectTypes", "Types of the objects of the notifications of the module", false);
	I_END_COMPONENT;

	// reimplemented (imtnotify::INotificationTypeList)
	virtual int GetTypesCount() const override;
	virtual const INotificationTypeInfo* GetTypeInfo(int index) const override;
	virtual int GetObjectTypesCount() const override;
	virtual const INotificationTypeInfo* GetObjectTypeInfo(int index) const override;

private:
	I_MULTIREF(INotificationTypeInfo, m_typesCompPtr);
	I_MULTIREF(INotificationTypeInfo, m_objectTypesCompPtr);
};


} // namespace imtnotify


