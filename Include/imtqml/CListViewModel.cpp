// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CListViewModel.h>


namespace imtqml
{


// public methods

CListViewModel::CListViewModel(QObject* parent)
	:BaseClass(parent)
{
}


bool CListViewModel::IsDirty() const
{
	return m_isDirty;
}


void CListViewModel::SetSourceValues(const QVariantList& rows)
{
	m_isSourceUpdate = true;

	beginResetModel();

	m_rows.clear();
	m_rows.reserve(rows.size());
	for (const QVariant& element: rows){
		m_rows.append(element);
	}

	RebuildRoles();

	endResetModel();

	m_isSourceUpdate = false;

	m_snapshot = GetValues();
	SetIsDirty(false);
	Q_EMIT countChanged();
}


QVariantList CListViewModel::GetValues() const
{
	QVariantList retVal;
	retVal.reserve(m_rows.size());
	for (const QVariant& element: m_rows){
		retVal.append(element);
	}

	return retVal;
}


void CListViewModel::MarkClean()
{
	m_snapshot = GetValues();
	SetIsDirty(false);
}


// reimplemented (QAbstractListModel)

int CListViewModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()){
		return 0;
	}

	return int(m_rows.size());
}


QVariant CListViewModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0 || index.row() >= int(m_rows.size())){
		return QVariant();
	}

	const QVariant& element = m_rows.at(index.row());

	if (role == ModelDataRole){
		return element;
	}

	const QByteArray roleName = m_roleNames.value(role);
	if (roleName.isEmpty()){
		return QVariant();
	}

	if (element.typeId() == QMetaType::QVariantMap){
		return element.toMap().value(QString::fromUtf8(roleName));
	}

	return QVariant();
}


bool CListViewModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.row() < 0 || index.row() >= int(m_rows.size())){
		return false;
	}

	if (role == ModelDataRole){
		if (m_rows.at(index.row()) == value){
			return true;
		}
		m_rows[index.row()] = value;
		Q_EMIT dataChanged(index, index);
		OnUserEdit();
		return true;
	}

	const QByteArray roleName = m_roleNames.value(role);
	if (roleName.isEmpty()){
		return false;
	}

	setProperty(index.row(), QString::fromUtf8(roleName), value);

	return true;
}


QHash<int, QByteArray> CListViewModel::roleNames() const
{
	return m_roleNames;
}


// public slots

QVariantMap CListViewModel::get(int row) const
{
	if (row < 0 || row >= int(m_rows.size())){
		return QVariantMap();
	}

	const QVariant& element = m_rows.at(row);
	if (element.typeId() == QMetaType::QVariantMap){
		return element.toMap();
	}

	QVariantMap retVal;
	retVal.insert(QStringLiteral("modelData"), element);

	return retVal;
}


void CListViewModel::setProperty(int row, const QString& key, const QVariant& value)
{
	if (row < 0 || row >= int(m_rows.size())){
		return;
	}

	QVariantMap map = m_rows.at(row).toMap();
	if (map.value(key) == value){
		return;
	}

	map.insert(key, value);
	m_rows[row] = map;

	bool rolesChanged = RegisterKeys(map);

	const QModelIndex changedIndex = index(row);
	Q_EMIT dataChanged(changedIndex, changedIndex);
	if (rolesChanged){
		// New roles require a structural refresh for existing delegates.
		beginResetModel();
		endResetModel();
	}

	OnUserEdit();
}


void CListViewModel::append(const QVariant& element)
{
	insert(int(m_rows.size()), element);
}


void CListViewModel::insert(int row, const QVariant& element)
{
	int clampedRow = row;
	if (clampedRow < 0){
		clampedRow = 0;
	}
	if (clampedRow > int(m_rows.size())){
		clampedRow = int(m_rows.size());
	}

	bool rolesChanged = false;
	if (element.typeId() == QMetaType::QVariantMap){
		rolesChanged = RegisterKeys(element.toMap());
	}

	if (rolesChanged){
		beginResetModel();
		m_rows.insert(clampedRow, element);
		endResetModel();
	}
	else{
		beginInsertRows(QModelIndex(), clampedRow, clampedRow);
		m_rows.insert(clampedRow, element);
		endInsertRows();
	}

	Q_EMIT countChanged();
	OnUserEdit();
}


void CListViewModel::remove(int row)
{
	if (row < 0 || row >= int(m_rows.size())){
		return;
	}

	beginRemoveRows(QModelIndex(), row, row);
	m_rows.remove(row);
	endRemoveRows();

	Q_EMIT countChanged();
	OnUserEdit();
}


void CListViewModel::revert()
{
	SetSourceValues(m_snapshot);
}


// private methods

void CListViewModel::RebuildRoles()
{
	m_roleNames.clear();
	m_roleIds.clear();
	m_nextRoleId = DynamicRole;

	m_roleNames.insert(ModelDataRole, QByteArrayLiteral("modelData"));

	for (const QVariant& element: m_rows){
		if (element.typeId() == QMetaType::QVariantMap){
			RegisterKeys(element.toMap());
		}
	}
}


bool CListViewModel::RegisterKeys(const QVariantMap& map)
{
	bool added = false;

	for (auto it = map.constBegin(); it != map.constEnd(); ++it){
		if (!m_roleIds.contains(it.key())){
			const int roleId = m_nextRoleId++;
			m_roleIds.insert(it.key(), roleId);
			m_roleNames.insert(roleId, it.key().toUtf8());
			added = true;
		}
	}

	return added;
}


void CListViewModel::SetIsDirty(bool isDirty)
{
	if (m_isDirty != isDirty){
		m_isDirty = isDirty;
		Q_EMIT isDirtyChanged(m_isDirty);
	}
}


void CListViewModel::OnUserEdit()
{
	if (m_isSourceUpdate){
		return;
	}

	SetIsDirty(true);
	Q_EMIT changed();
}


} // namespace imtqml
