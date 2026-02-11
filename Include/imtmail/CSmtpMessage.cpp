// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmail/CSmtpMessage.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtmail
{


// public methods

QString CSmtpMessage::GetFrom() const
{
	return m_from;
}


void CSmtpMessage::SetFrom(const QString& from)
{
	if (m_from != from){
		istd::CChangeNotifier changeNotifier(this);

		m_from = from;
	}
}


QString CSmtpMessage::GetTo() const
{
	return m_to;
}


void CSmtpMessage::SetTo(const QString& to)
{
	if (m_to != to){
		istd::CChangeNotifier changeNotifier(this);

		m_to = to;
	}
}


QString CSmtpMessage::GetSubject() const
{
	return m_subject;
}


void CSmtpMessage::SetSubject(const QString& subject)
{
	if (m_subject != subject){
		istd::CChangeNotifier changeNotifier(this);

		m_subject = subject;
	}
}


QString CSmtpMessage::GetBody() const
{
	return m_body;
}


void CSmtpMessage::SetBody(const QString& body)
{
	if (m_body != body){
		istd::CChangeNotifier changeNotifier(this);

		m_body = body;
	}
}


// reimplemented (iser::ISerializable)

bool CSmtpMessage::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag fromTag("From", "From", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fromTag);
	retVal = retVal && archive.Process(m_from);
	retVal = retVal && archive.EndTag(fromTag);

	iser::CArchiveTag toTag("To", "To", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(toTag);
	retVal = retVal && archive.Process(m_to);
	retVal = retVal && archive.EndTag(toTag);

	iser::CArchiveTag subjectTag("Subject", "Subject", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(subjectTag);
	retVal = retVal && archive.Process(m_subject);
	retVal = retVal && archive.EndTag(subjectTag);

	iser::CArchiveTag bodyTag("Body", "Body", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(bodyTag);
	retVal = retVal && archive.Process(m_body);
	retVal = retVal && archive.EndTag(bodyTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CSmtpMessage::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CSmtpMessage* sourcePtr = dynamic_cast<const CSmtpMessage*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_to = sourcePtr->m_to;
		m_from = sourcePtr->m_from;
		m_subject = sourcePtr->m_subject;
		m_body = sourcePtr->m_body;

		return true;
	}

	return false;
}


bool CSmtpMessage::IsEqual(const IChangeable& object) const
{
	bool retVal = true;
	const CSmtpMessage* sourcePtr = dynamic_cast<const CSmtpMessage*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_to == sourcePtr->m_to;
		retVal = retVal && m_from == sourcePtr->m_from;
		retVal = retVal && m_subject == sourcePtr->m_subject;
		retVal = retVal && m_body == sourcePtr->m_body;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CSmtpMessage::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CSmtpMessage);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CSmtpMessage::ResetData(CompatibilityMode /*mode*/)
{
	m_to.clear();
	m_from.clear();
	m_subject.clear();
	m_body.clear();

	return true;
}


} // namespace imtmail
