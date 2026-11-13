// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtnotify/INotificationTypeInfo.h>


namespace imtnotify
{


/**
	A notification type, or a type of the objects of notifications, declared in an .acc file.
	The label and the message are texts of the .acc file, translated in the context "Attribute".
	A message that is not valid or uses an undeclared argument is logged when the component is created.
*/
class CNotificationTypeComp:
			public ilog::CLoggerComponentBase,
			virtual public INotificationTypeInfo
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationTypeComp);
		I_REGISTER_INTERFACE(INotificationTypeInfo);
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Id of the type", true, "");
		I_ASSIGN(m_nameAttrPtr, "Name", "Label of the type", true, "");
		I_ASSIGN(m_iconPathAttrPtr, "IconPath", "Path of the icon of the type, empty for a type of objects", false, "");
		I_ASSIGN(m_messageAttrPtr, "Message", "Message the client shows, an ICU MessageFormat pattern", false, "");
		I_ASSIGN_MULTI_0(m_parameterIdsAttrPtr, "ParameterIds", "Arguments the message uses besides subject, actor, actors and count", false);
	I_END_COMPONENT;

	// reimplemented (imtnotify::INotificationTypeInfo)
	virtual QByteArray GetTypeId() const override;
	virtual QString GetName() const override;
	virtual QByteArray GetIconPath() const override;
	virtual QString GetMessage() const override;
	virtual QByteArrayList GetParameterIds() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_typeIdAttrPtr);
	I_TEXTATTR(m_nameAttrPtr);
	I_ATTR(QByteArray, m_iconPathAttrPtr);
	I_TEXTATTR(m_messageAttrPtr);
	I_MULTIATTR(QByteArray, m_parameterIdsAttrPtr);
};


} // namespace imtnotify


