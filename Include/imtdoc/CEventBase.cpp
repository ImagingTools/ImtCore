// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


// public methods

QByteArray CEventBase::Type() const
{
	return QByteArrayLiteral("BaseEvent");
}
	

bool CEventBase::IsAccepted() const
{
	return m_isAccepted;
}


void CEventBase::Accept()
{
	m_isAccepted = true;
}


void CEventBase::Reject()
{
	m_isAccepted = false;
}


QByteArray CEventBase::GetUserId() const
{
	return m_userId;
}


QByteArray CEventBase::GetDocumentId() const
{
	return m_documentId;
}


QByteArray CEventBase::GetDocumentTypeId() const
{
	return m_documentTypeId;
}


QString CEventBase::GetDocumentName() const
{
	return m_documentName;
}


QUrl CEventBase::GetDocumentUrl() const
{
	return m_documentUrl;
}


bool CEventBase::IsDocumentDirty() const
{
	return m_isDirty;
}


} // namespace imtdoc


