// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoas/CReferenceObject.h>


namespace imtoas
{


QString CReferenceObject::GetReference() const
{
	return m_reference;
}


void CReferenceObject::SetReference(const QString& reference)
{
	if (reference != m_reference){
		m_reference = reference;
	}
}


QString CReferenceObject::GetSummary() const
{
	return m_summary;
}


void CReferenceObject::SetSummary(const QString& summary)
{
	if (summary != m_summary){
		m_summary = summary;
	}
}


QString CReferenceObject::GetDescription() const
{
	return m_description;
}


void CReferenceObject::SetDescription(const QString& description)
{
	if (description != m_description){
		m_description = description;
	}
}


bool CReferenceObject::ReadFromJsonObject(CReferenceObject& object, const QJsonObject& jsonObject, const QJsonObject& /*globalObject*/)
{
	QVariant referenceData = jsonObject.value("$ref").toVariant();
	if (referenceData.isNull()){
		return false;
	}
	object.SetReference(referenceData.toString());

	QVariant summaryData = jsonObject.value("summary").toVariant();
	if (!summaryData.isNull()){
		object.SetSummary(summaryData.toString());
	}

	QVariant descriptionData = jsonObject.value("description").toVariant();
	if (!descriptionData.isNull()){
		object.SetDescription(descriptionData.toString());
	}

	return true;
}


bool CReferenceObject::operator==(const CReferenceObject& other) const
{
	bool retVal = true;
	retVal = retVal && (m_reference == other.m_reference);
	retVal = retVal && (m_summary == other.m_summary);
	retVal = retVal && (m_description == other.m_description);

	return retVal;
}


} // namespace imtoas

