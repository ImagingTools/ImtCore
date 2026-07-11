// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CObjectViewModel.h>


namespace imtqml
{


// public methods

CObjectViewModel::CObjectViewModel(QObject* parent)
	:BaseClass(parent)
{
}


bool CObjectViewModel::IsDirty() const
{
	return m_isDirty;
}


void CObjectViewModel::SetSourceValues(const QVariantMap& values)
{
	m_isSourceUpdate = true;

	for (auto it = values.constBegin(); it != values.constEnd(); ++it){
		if (value(it.key()) != it.value()){
			insert(it.key(), it.value());
		}
	}

	m_isSourceUpdate = false;

	m_snapshot = GetValues();
	m_changedValues.clear();
	SetIsDirty(false);
}


QVariantMap CObjectViewModel::GetValues() const
{
	QVariantMap retVal;

	const QStringList propertyKeys = keys();
	for (const QString& key: propertyKeys){
		retVal.insert(key, value(key));
	}

	return retVal;
}


QVariantMap CObjectViewModel::GetChangedValues() const
{
	return m_changedValues;
}


void CObjectViewModel::MarkClean()
{
	m_snapshot = GetValues();
	m_changedValues.clear();
	SetIsDirty(false);
}


// public slots

void CObjectViewModel::revert()
{
	SetSourceValues(m_snapshot);
}


// protected methods

// reimplemented (QQmlPropertyMap)

QVariant CObjectViewModel::updateValue(const QString& key, const QVariant& input)
{
	if (!m_isSourceUpdate){
		m_changedValues.insert(key, input);
		SetIsDirty(true);

		Q_EMIT valueEdited(key, input);
	}

	return input;
}


// private methods

void CObjectViewModel::SetIsDirty(bool isDirty)
{
	if (m_isDirty != isDirty){
		m_isDirty = isDirty;
		Q_EMIT isDirtyChanged(m_isDirty);
	}
}


} // namespace imtqml
