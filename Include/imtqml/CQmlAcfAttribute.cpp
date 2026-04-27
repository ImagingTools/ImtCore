// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlAcfAttribute.h>


namespace imtqml
{


CQmlAcfAttribute::CQmlAcfAttribute(QObject* parent)
	: QObject(parent)
{
}


const QString& CQmlAcfAttribute::GetAttrId() const
{
	return m_attrId;
}


void CQmlAcfAttribute::SetAttrId(const QString& attrId)
{
	if (m_attrId != attrId){
		m_attrId = attrId;
		Q_EMIT attrIdChanged();
	}
}


const QString& CQmlAcfAttribute::GetType() const
{
	return m_type;
}


void CQmlAcfAttribute::SetType(const QString& type)
{
	if (m_type != type){
		m_type = type;
		Q_EMIT typeChanged();
	}
}


bool CQmlAcfAttribute::IsRequired() const
{
	return m_required;
}


void CQmlAcfAttribute::SetRequired(bool required)
{
	if (m_required != required){
		m_required = required;
		Q_EMIT requiredChanged();
	}
}


const QVariant& CQmlAcfAttribute::GetDefaultValue() const
{
	return m_defaultValue;
}


void CQmlAcfAttribute::SetDefaultValue(const QVariant& defaultValue)
{
	if (m_defaultValue != defaultValue){
		m_defaultValue = defaultValue;
		Q_EMIT defaultValueChanged();
	}
}


const QVariant& CQmlAcfAttribute::GetValue() const
{
	return m_value;
}


void CQmlAcfAttribute::SetValue(const QVariant& value)
{
	if (m_value != value){
		m_value = value;
		Q_EMIT valueChanged();
	}
}


const QString& CQmlAcfAttribute::GetDescription() const
{
	return m_description;
}


void CQmlAcfAttribute::SetDescription(const QString& description)
{
	if (m_description != description){
		m_description = description;
		Q_EMIT descriptionChanged();
	}
}


QVariant CQmlAcfAttribute::resolvedValue() const
{
	if (m_value.isValid()){
		return m_value;
	}

	return m_defaultValue;
}


} // namespace imtqml
