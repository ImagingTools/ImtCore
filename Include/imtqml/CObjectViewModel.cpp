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


bool CObjectViewModel::HasErrors() const
{
	for (auto it = m_fieldErrors.constBegin(); it != m_fieldErrors.constEnd(); ++it){
		if (!it.value().toString().isEmpty()){
			return true;
		}
	}

	return false;
}


void CObjectViewModel::SetFieldErrors(const QVariantMap& errors)
{
	if (m_fieldErrors == errors){
		return;
	}

	m_fieldErrors = errors;

	Q_EMIT fieldErrorsChanged();
}


void CObjectViewModel::SetSourceValues(const QVariantMap& values)
{
	m_isSourceUpdate = true;

	for (auto it = values.constBegin(); it != values.constEnd(); ++it){
		if (it.value().typeId() == QMetaType::QVariantList){
			CListViewModel* adapter = GetOrCreateListAdapter(it.key());
			adapter->SetSourceValues(it.value().toList());
			continue;
		}

		if (value(it.key()) != it.value()){
			insert(it.key(), it.value());
		}
	}

	m_isSourceUpdate = false;

	m_snapshot = GetValues();
	m_changedValues.clear();
	SetIsDirty(false);

	if (!m_fieldErrors.isEmpty()){
		m_fieldErrors.clear();
		Q_EMIT fieldErrorsChanged();
	}
}


CListViewModel* CObjectViewModel::GetListAdapter(const QString& key) const
{
	return m_listAdapters.value(key, nullptr);
}


QVariantMap CObjectViewModel::GetValues() const
{
	QVariantMap retVal;

	const QStringList propertyKeys = keys();
	for (const QString& key: propertyKeys){
		CListViewModel* adapter = m_listAdapters.value(key, nullptr);
		if (adapter != nullptr){
			retVal.insert(key, adapter->GetValues());
		}
		else{
			retVal.insert(key, value(key));
		}
	}

	return retVal;
}


QVariantMap CObjectViewModel::GetChangedValues() const
{
	return m_changedValues;
}


void CObjectViewModel::MarkClean()
{
	for (CListViewModel* adapter: m_listAdapters){
		adapter->MarkClean();
	}

	m_snapshot = GetValues();
	m_changedValues.clear();
	SetIsDirty(false);
}


// public slots

void CObjectViewModel::revert()
{
	SetSourceValues(m_snapshot);
}


QString CObjectViewModel::fieldError(const QString& key) const
{
	return m_fieldErrors.value(key).toString();
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


CListViewModel* CObjectViewModel::GetOrCreateListAdapter(const QString& key)
{
	CListViewModel* adapter = m_listAdapters.value(key, nullptr);
	if (adapter != nullptr){
		return adapter;
	}

	adapter = new CListViewModel(this);
	m_listAdapters.insert(key, adapter);

	connect(adapter, &CListViewModel::changed, this, [this, key](){
		OnListAdapterChanged(key);
	});

	// Expose the adapter object as the property value so QML views can
	// bind to model.<key>; done once, so bindings stay stable across
	// source updates.
	insert(key, QVariant::fromValue(adapter));

	return adapter;
}


void CObjectViewModel::OnListAdapterChanged(const QString& key)
{
	CListViewModel* adapter = m_listAdapters.value(key, nullptr);
	if (adapter == nullptr){
		return;
	}

	const QVariant values = QVariant::fromValue(adapter->GetValues());
	m_changedValues.insert(key, values);
	SetIsDirty(true);

	Q_EMIT valueEdited(key, values);
}


} // namespace imtqml
