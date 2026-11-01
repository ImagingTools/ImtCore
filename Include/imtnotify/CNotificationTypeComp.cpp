// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/CNotificationTypeComp.h>


// ImtCore includes
#include <imtnotify/NotificationMessageFormat.h>


namespace imtnotify
{


// reimplemented (imtnotify::INotificationTypeInfo)

QByteArray CNotificationTypeComp::GetTypeId() const
{
	return *m_typeIdAttrPtr;
}


QString CNotificationTypeComp::GetName() const
{
	// The source text: the server translates it into the language of the client, not of the server
	return m_nameAttrPtr.GetOriginalValue();
}


QByteArray CNotificationTypeComp::GetIconPath() const
{
	return m_iconPathAttrPtr.IsValid() ? *m_iconPathAttrPtr : QByteArray();
}


QString CNotificationTypeComp::GetMessage() const
{
	return m_messageAttrPtr.IsValid() ? m_messageAttrPtr.GetOriginalValue() : QString();
}


QByteArrayList CNotificationTypeComp::GetParameterIds() const
{
	QByteArrayList retVal;
	for (int i = 0; i < m_parameterIdsAttrPtr.GetCount(); ++i){
		retVal.append(m_parameterIdsAttrPtr[i]);
	}

	return retVal;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CNotificationTypeComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const QString message = GetMessage();
	if (message.isEmpty()){
		return;
	}

	QSet<QByteArray> argumentIds;
	QString errorMessage;
	if (!NotificationMessageFormat::GetArgumentIds(message, argumentIds, errorMessage)){
		SendErrorMessage(0, QStringLiteral("The message of notification type '%1' is not valid: %2").arg(QString::fromUtf8(GetTypeId()), errorMessage), "CNotificationTypeComp");

		return;
	}

	const QByteArrayList parameterIds = GetParameterIds();
	for (const QByteArray& argumentId : std::as_const(argumentIds)){
		if (!NotificationMessageFormat::s_standardArgumentIds.contains(argumentId) && !parameterIds.contains(argumentId)){
			SendErrorMessage(0, QStringLiteral("The message of notification type '%1' uses the undeclared argument '%2'")
						.arg(QString::fromUtf8(GetTypeId()), QString::fromUtf8(argumentId)),
						"CNotificationTypeComp");
		}
	}
}


} // namespace imtnotify


