// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CTreeItemModel.h>


// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CJsonMemWriteArchive.h>
#include <istd/TDelPtr.h>


namespace imtbase
{


CTreeItemModel::CTreeItemModel(QObject* parent)
	:QAbstractListModel(parent),
	m_isArray(false),
	m_isUpdateEnabled(true)
{
	m_countChanges = 0;
	m_countTransaction = 0;
}


CTreeItemModel::~CTreeItemModel()
{
	Clear();
}


const QString& CTreeItemModel::State() const
{
	return m_state;
}


void CTreeItemModel::SetState(const QString& newState)
{
	if (m_state != newState){
		m_state = newState;

		emit stateChanged(m_state);
	}
}


// public slots

void CTreeItemModel::SetParent(QObject* parent)
{
	BaseClass2::setParent(parent);
}


bool CTreeItemModel::Copy(const CTreeItemModel* object)
{
	if (object == nullptr){
		return false;
	}

	beginResetModel();

	qDeleteAll(m_items);

	m_items.clear();

	for (int i = 0; i < object->m_items.count(); i++){
		Item* item = new Item();
		Item* sourceItem = object->m_items[i];

		QList<QByteArray> keys;
		sourceItem->GetKeys(keys);

		for (const QByteArray& key : std::as_const(keys)){
			QVariant value = sourceItem->Value(key);

			CTreeItemModel* treeItemModelPtr = nullptr;
			if (value.isValid()){
				treeItemModelPtr = value.value<CTreeItemModel*>();
			}

			if (treeItemModelPtr != nullptr){
				CTreeItemModel* subModelPtr = new CTreeItemModel();

				if (!subModelPtr->Copy(treeItemModelPtr)){
					delete item;
					delete subModelPtr;
					endResetModel();

					return false;
				}

				subModelPtr->setParent(this);

				item->SetValue(key, QVariant::fromValue(subModelPtr));
			}
			else {
				item->SetValue(key, value);
			}
		}

		m_items.append(item);
	}

	m_roleNames = object->m_roleNames;
	m_queryParams = object->m_queryParams;
	m_isArray = object->m_isArray;
	m_state = object->m_state;

	endResetModel();

	OnModelChanged();

	return true;
}


imtbase::CTreeItemModel* CTreeItemModel::CopyMe() const
{
	istd::TDelPtr<CTreeItemModel> resultModelPtr = new CTreeItemModel();
	if (resultModelPtr->Copy(this)){
		return resultModelPtr.PopPtr();
	}

	return nullptr;
}


bool CTreeItemModel::IsEqualWithModel(CTreeItemModel* modelPtr) const
{
	if (modelPtr == nullptr){
		return false;
	}

	if (m_items.size() != modelPtr->m_items.size()){
		return false;
	}

	for (int i = 0; i < modelPtr->m_items.count(); i++){
		Item* item = m_items[i];
		Item* sourceItem = modelPtr->m_items[i];

		QList<QByteArray> itemKeys;
		item->GetKeys(itemKeys);

		QList<QByteArray> keys;
		sourceItem->GetKeys(keys);

		if (itemKeys.size() != keys.size()){
			return false;
		}

		for (const QByteArray& key : std::as_const(keys)){
			if (!itemKeys.contains(key)){
				return false;
			}

			QVariant sourceValue = sourceItem->Value(key);
			QVariant itemValue = item->Value(key);

			CTreeItemModel* sourceTreeItemModelPtr = sourceValue.value<CTreeItemModel*>();
			if (sourceTreeItemModelPtr != nullptr){
				CTreeItemModel* treeItemModelPtr = itemValue.value<CTreeItemModel*>();
				if (treeItemModelPtr == nullptr){
					return false;
				}

				bool result = sourceTreeItemModelPtr->IsEqualWithModel(treeItemModelPtr);
				if (!result){
					return false;
				}
			}
			else{
#if QT_VERSION < 0x060000
				if (sourceValue.type() == QMetaType::Int && sourceValue.toInt() != itemValue.toInt()){
#else
				if (sourceValue.typeId() == QMetaType::Int && sourceValue.toInt() != itemValue.toInt()){
#endif
					return false;
				}
#if QT_VERSION < 0x060000
				if (sourceValue.type() == QMetaType::QByteArray && sourceValue.toByteArray() != itemValue.toByteArray()){
#else
				if (sourceValue.typeId() == QMetaType::QByteArray && sourceValue.toByteArray() != itemValue.toByteArray()){
#endif
					return false;
				}
#if QT_VERSION < 0x060000
				if (sourceValue.type() == QMetaType::QString && sourceValue.toString() != itemValue.toString()){
#else
				if (sourceValue.typeId() == QMetaType::QString && sourceValue.toString() != itemValue.toString()){
#endif
					return false;
				}
#if QT_VERSION < 0x060000
				if (sourceValue.type() == QMetaType::Bool && sourceValue.toBool() != itemValue.toBool()){
#else
				if (sourceValue.typeId() == QMetaType::Bool && sourceValue.toBool() != itemValue.toBool()){
#endif
					return false;
				}
			}
		}
	}

	return true;
}


bool CTreeItemModel::IsEqualWithModel(const CTreeItemModel* modelPtr) const
{
	CTreeItemModel* notConstModelPtr = const_cast<CTreeItemModel*>(modelPtr);
	if (notConstModelPtr == nullptr){
		return false;
	}

	return IsEqualWithModel(notConstModelPtr);
}


void CTreeItemModel::InsertNewItemWithParameters(int index, const QVariantMap& map)
{
	if (index < 0 || index > m_items.count()){
		return;
	}

	beginInsertRows(QModelIndex(), index, index);

	m_items.insert(index, new Item());

	for(auto value = map.cbegin(); value != map.cend(); ++ value){
		CTreeItemModel* treeItemModelPtr = nullptr;
		if ((*value).isValid()){
			treeItemModelPtr = (*value).value<CTreeItemModel*>();
		}

		if (treeItemModelPtr != nullptr){
			treeItemModelPtr->SetParent(this);
		}

		QByteArray key = value.key().toUtf8();

		QList<QByteArray> roles = m_roleNames.values();
		if (!roles.contains(key)){
			m_roleNames.insert(Qt::UserRole + 1 + m_roleNames.count(), key);
		}

		Item* itemPtr = m_items[index];
		if (itemPtr != nullptr){
			itemPtr->SetValue(key,* value);
		}
	}

	endInsertRows();

	OnModelChanged();
}


int CTreeItemModel::InsertNewItem()
{
	int index = m_items.size();

	if(m_isUpdateEnabled){
		beginInsertRows(QModelIndex(), index, index);
	}

	m_items.append(new Item());

	if(m_items.count() > 1){
		m_isArray = true;
	}

	if(m_isUpdateEnabled){
		endInsertRows();
	}

	if(m_isUpdateEnabled){
		OnModelChanged();
	}

	return index;
}


int CTreeItemModel::InsertNewItem(int index)
{
	if (index < 0 || index > m_items.count()){
		return -1;
	}

	if(m_isUpdateEnabled){
		beginInsertRows(QModelIndex(), index, index);
	}

	m_items.insert(index, new Item());

	if(m_items.count() > 1){
		m_isArray = true;
	}

	if(m_isUpdateEnabled){
		endInsertRows();
	}

	if(m_isUpdateEnabled){
		OnModelChanged();
	}

	return index;
}


int CTreeItemModel::RemoveItem(int index)
{
	if (index < 0 || index > m_items.count() - 1){
		return -1;
	}

	if(m_isUpdateEnabled){
		beginRemoveRows(QModelIndex(), index, index);
	}

	Item* item = m_items.takeAt(index);
	delete item;

	if(m_isUpdateEnabled){
		endRemoveRows();
	}

	if(m_items.isEmpty()){
		m_isArray = false;
	}

	if(m_isUpdateEnabled){
		OnModelChanged();
	}

	return index;
}

bool CTreeItemModel::SwapItems(int index1, int index2)
{
	if (index1 < 0 || index1 >= m_items.count() || index2 < 0 || index2 >= m_items.count()){
		return false;
	}

	if (index1 == index2){
		return true;
	}

	if(m_isUpdateEnabled){
		beginResetModel();
	}

	m_items.swapItemsAt(index1, index2);

	if(m_isUpdateEnabled){
		endResetModel();
	}

	return true;
}


imtbase::CTreeItemModel* CTreeItemModel::AddTreeModel(const QByteArray& key, int index)
{
	if(m_isUpdateEnabled){
		BeginChanges();
	}

	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		if(m_isUpdateEnabled){
			EndChanges();
		}

		return nullptr;
	}

	CTreeItemModel* retVal = GetTreeItemModel(key, index);
	if (retVal == nullptr){
		retVal = new CTreeItemModel(this);

		QVariant v = QVariant::fromValue(retVal);

		SetData(key, v, index);
	}
	else{
		retVal->Clear();
	}

	if(m_isUpdateEnabled){
		EndChanges();
	}

	return retVal;
}


bool CTreeItemModel::SetExternTreeModel(const QByteArray& key, CTreeItemModel* externTreeModel, int index)
{
	if(m_isUpdateEnabled){
		BeginChanges();
	}

	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		if(m_isUpdateEnabled){
			EndChanges();
		}

		return false;
	}

	if (externTreeModel != nullptr){
		externTreeModel->setParent(this);
	}

	QVariant v = QVariant::fromValue(externTreeModel);
	SetData(key, v, index);

	if(m_isUpdateEnabled){
		EndChanges();
	}

	return true;
}


bool CTreeItemModel::CopyItemDataFromModel(int index, CTreeItemModel* externTreeModel, int externIndex)
{
	const CTreeItemModel* constExternTreeModel = externTreeModel;

	return CopyItemDataFromModel(index, constExternTreeModel, externIndex);
}


bool CTreeItemModel::CopyItemDataFromModel(int index, const CTreeItemModel* externTreeModel, int externIndex)
{
	if(m_isUpdateEnabled){
		BeginChanges();
	}

	RemoveItem(index);
	InsertNewItem(index);

	bool retVal = true;
	QList<QByteArray> keys;
	externTreeModel->GetKeys(keys, externIndex);
	for (const QByteArray& key : std::as_const(keys)){
		QVariant value = externTreeModel->GetData(key, externIndex);

		CTreeItemModel* treeItemModelPtr = nullptr;
		if (value.isValid()){
			treeItemModelPtr = value.value<CTreeItemModel*>();
		}
		if (treeItemModelPtr != nullptr){
			CTreeItemModel* childModelPtr = AddTreeModel(key, index);

			retVal = retVal && childModelPtr->Copy(treeItemModelPtr);
		}
		else{
			retVal = retVal && SetData(key, value, index);
		}

		if (!retVal){
			break;
		}
	}

	if(m_isUpdateEnabled){
		EndChanges();
	}

	return retVal;
}


bool CTreeItemModel::CopyItemDataToModel(int index, CTreeItemModel* externTreeModel, int externIndex) const
{
	bool retVal = false;
	QList<QByteArray> keys;
	this->GetKeys(keys, index);

	for (const QByteArray& key : std::as_const(keys)){
		QVariant value = GetData(key, index);
		retVal = externTreeModel->SetData(key, value, externIndex);

		if (!retVal){
			break;
		}
	}

	return retVal;
}


bool CTreeItemModel::SetData(
		const QByteArray& key,
		const QVariant& value,
		int index)
{
	if(m_isUpdateEnabled){
		BeginChanges();
	}

	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		if(m_isUpdateEnabled){
			EndChanges();
		}
		return false;
	}

	QList<QByteArray> roles = m_roleNames.values();
	if (!roles.contains(key)){
		m_roleNames.insert(Qt::UserRole + 1 + m_roleNames.count(), key);
	}

	Item* item = m_items[index];

	if (item->Value(key) == value){
		if(m_isUpdateEnabled){
			EndChanges();
		}
		return true;
	}

	item->SetValue(key, value);

	if(m_isUpdateEnabled){
		int keyRole = GetKeyRole(key);
		if (keyRole > -1){
			QVector<int> keyRoles;
			keyRoles.append(keyRole);

			Q_EMIT dataChanged(QAbstractListModel::index(index), QAbstractListModel::index(index), keyRoles);
		}

		OnModelChanged();

		EndChanges();
	}

	return true;
}


bool CTreeItemModel::RemoveData(const QByteArray& key, int index)
{
	if (index < 0 || index > m_items.count() - 1){
		return false;
	}

	Item* item = m_items[index];

	item->RemoveValue(key);

	int keyRole = GetKeyRole(key);
	if (keyRole > -1){
		QVector<int> roles;
		roles.append(keyRole);

		if(m_isUpdateEnabled){
			Q_EMIT dataChanged(QAbstractListModel::index(index), QAbstractListModel::index(index), roles);
		}
	}

	if(m_isUpdateEnabled){
		OnModelChanged();
	}

	return true;
}


QVariant CTreeItemModel::GetData(const QByteArray& key, int index) const
{
	if (index < 0 || index > m_items.count() - 1){
		return QVariant();
	}

	return m_items[index]->Value(key);
}


imtbase::CTreeItemModel* CTreeItemModel::GetParent() const
{
	return dynamic_cast<CTreeItemModel*>(this->parent());
}


bool CTreeItemModel::IsTreeModel(const QByteArray& key, int index) const
{
	return GetTreeItemModel(key,index) != nullptr;
}


bool CTreeItemModel::ContainsKey(const QByteArray& key, int index) const
{
	bool retVal = false;

	if (index >= 0 && index < m_items.count()){
		retVal = m_items[index]->ContainsKey(key);
	}

	return retVal;
}


bool CTreeItemModel::IsValidData(const QByteArray& key, int index) const
{
	return GetData(key, index) != QVariant();
}


imtbase::CTreeItemModel* CTreeItemModel::GetTreeItemModel(const QByteArray& key, int index) const
{
	QVariant data = GetData(key, index);
	if (data.isValid()){
		return data.value<CTreeItemModel*>();
	}

	return nullptr;
}


CTreeItemModel* CTreeItemModel::GetModelFromItem(int itemIndex) const
{
	int itemCount = GetItemsCount();
	if (itemCount <= itemIndex){
		return nullptr;
	}

	istd::TDelPtr<CTreeItemModel> modelPtr(new CTreeItemModel());

	bool result = modelPtr->CopyItemDataFromModel(0, this, itemIndex);
	if (result){
		return modelPtr.PopPtr();
	}

	return nullptr;
}


int CTreeItemModel::GetItemsCount() const
{
	return m_items.size();
}


void CTreeItemModel::GetKeys(QList<QByteArray>& keys, int index) const
{
	if (index >= 0 && index < m_items.count()){
		m_items[index]->GetKeys(keys);
	}
}


QList<QString> CTreeItemModel::GetKeys(int index) const
{
	QList<QByteArray> keys;
	if (index >= 0 && index < m_items.count()){
		m_items[index]->GetKeys(keys);
	}

	QList<QString> keys2;
	for (const QByteArray& key : std::as_const(keys)){
		keys2.append(key);
	}

	return keys2;
}


void CTreeItemModel::Clear()
{
	if(m_isUpdateEnabled){
		beginResetModel();
	}

	qDeleteAll(m_items);
	m_items.clear();

	if(m_isUpdateEnabled){
		endResetModel();

		OnModelChanged();
	}
}


bool CTreeItemModel::IsArray()
{
	return m_isArray;
}


void CTreeItemModel::SetIsArray(const bool& isArray)
{
	m_isArray = isArray;
}


bool CTreeItemModel::CreateFromJson(const QByteArray& jsonContent)
{
	if(m_isUpdateEnabled){
		BeginChanges();
	}

	Clear();

	QJsonParseError error;
	const QJsonDocument document = QJsonDocument::fromJson(jsonContent, &error);
	if (error.error != QJsonParseError::NoError){
		qCritical() << "Error during parsing JSON document:" << error.errorString() << "content:" << jsonContent;
		if(m_isUpdateEnabled){
			EndChanges();
		}

		return false;
	}

	bool retVal = true;

	if (document.isArray()){
		const QJsonArray jsonArray = document.array();

		for (const QJsonValue& value : jsonArray){
			if (value.isObject()){
				const int index = InsertNewItem();
				retVal = retVal && ParseRecursive(value.toObject(), index);
			}
			else if (value.isArray()){
				retVal = false;
			}
			else{
				const int index = InsertNewItem();
				retVal = retVal && SetData(QByteArray(), value.toVariant(), index);
			}

			if (!retVal){
				break;
			}
		}
	}
	else if (document.isObject()){
		retVal = ParseRecursive(document.object(), 0);
	}
	else{
		retVal = false;
	}

	if(m_isUpdateEnabled){
		EndChanges();
	}

	return retVal;
}


void CTreeItemModel::SetQueryParam(const QByteArray& key, const QByteArray& value)
{
	m_queryParams.insert(key, value);
}


QByteArray CTreeItemModel::GetQueryParam(const QByteArray& key)
{
	return m_queryParams.value(key);
}


QByteArray CTreeItemModel::TakeQueryParam(const QByteArray& key)
{
	return m_queryParams.take(key);
}


QMap<QByteArray, QByteArray>& CTreeItemModel::GetQueryParams()
{
	return m_queryParams;
}


void CTreeItemModel::ClearQueryParams(const QByteArray& /*key*/)
{
	m_queryParams.clear();
}


void CTreeItemModel::SetUpdateEnabled(bool updateEnabled)
{
	m_isUpdateEnabled = updateEnabled;
}


void CTreeItemModel::OnModelChanged()
{
	if (m_countTransaction > 0){
		m_countChanges++;

		return;
	}

	if (m_isUpdateEnabled){
		Q_EMIT modelChanged();
	}

	CTreeItemModel* parentModelPtr = GetParent();
	if (parentModelPtr != nullptr){
		if (parentModelPtr->m_isUpdateEnabled){
			parentModelPtr->OnModelChanged();
		}
	}
}


void CTreeItemModel::Refresh()
{
	beginResetModel();
	endResetModel();
}


QString CTreeItemModel::ToJson()
{
	QByteArray representationData;

	iser::CJsonMemWriteArchive archive(nullptr, false);
	if (SerializeModel(archive)){
		representationData = archive.GetData();
	}
	else{
		Q_ASSERT(false);
		return QString();
	}

	if (representationData.length() > 2 && representationData[1] == '['){
		representationData = representationData.mid(1, representationData.length() - 2);
	}

	return QString::fromUtf8(representationData);
}


// reimplemented (QAbstractListModel)

int CTreeItemModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_items.size();
}


QVariant CTreeItemModel::data(const QModelIndex& index, int role) const
{
	if (!m_roleNames.contains(role)){
		return QVariant();
	}

	QByteArray key = m_roleNames.value(role);

	int row = index.row();

	return this->GetData(key, row);
}


bool CTreeItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!m_roleNames.contains(role)){
		return false;
	}

	QByteArray key = m_roleNames.value(role);
	int row = index.row();

	SetData(key, value, row);

	return true;
}


QHash<int, QByteArray> CTreeItemModel::roleNames() const
{
	return m_roleNames;
}


bool CTreeItemModel::SerializeModel(iser::IArchive& archive)
{
	if (!archive.IsStoring()){
		Clear();
	}

	return SerializeRecursive(archive, QByteArray());
}


bool CTreeItemModel::BeginChanges()
{
	m_countTransaction++;

	return true;
}


bool CTreeItemModel::EndChanges()
{
	m_countTransaction--;

	if (m_countChanges > 0 && m_countTransaction == 0){
		OnModelChanged();

		m_countChanges = 0;

		return true;
	}

	return false;
}


// protected methods

bool CTreeItemModel::SerializeRecursive(iser::IArchive& archive, const QByteArray& tagName)
{
	bool retVal = true;
	int countSize = m_items.count();

	iser::CArchiveTag arrayTag(tagName, QByteArrayLiteral("array items"), iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag subArrayTag(QByteArrayLiteral("Item"), QByteArrayLiteral("array item"), iser::CArchiveTag::TT_GROUP, &arrayTag);
	iser::CArchiveTag objectTag(tagName, QByteArrayLiteral("key"), iser::CArchiveTag::TT_GROUP);

	bool isMultiTag = (m_isArray || countSize > 1);

	const QList<QByteArray> roleKeys = m_roleNames.values();
	if (roleKeys.size() == 1 && roleKeys[0].isEmpty()){
		subArrayTag = iser::CArchiveTag(QByteArrayLiteral("Item"), QByteArrayLiteral("array item"), iser::CArchiveTag::TT_LEAF, &arrayTag);
	}

	if (!isMultiTag){
		if (!tagName.isEmpty()){
			retVal = retVal && archive.BeginTag(objectTag);
		}
	}
	else{
		retVal = retVal && archive.BeginMultiTag(arrayTag, subArrayTag, countSize);
	}

	for (int i = 0; i < countSize && retVal; ++i){
		Item* item = m_items[i];
		if (item == nullptr){
			retVal = false;
			break;
		}

		QList<QByteArray> itemKeys;
		item->GetKeys(itemKeys);

		const bool wrapSubItem = isMultiTag && !itemKeys.isEmpty() && !itemKeys[0].isEmpty();
		if (wrapSubItem){
			retVal = retVal && archive.BeginTag(subArrayTag);
		}

		for (const QByteArray& key : std::as_const(itemKeys)){
			const QVariant value = item->Value(key);

			if (value.isValid()){
				if (CTreeItemModel* treeItemModelPtr = value.value<CTreeItemModel*>()){
					retVal = retVal && treeItemModelPtr->SerializeRecursive(archive, key);
					if (!retVal){
						break;
					}
					continue;
				}
			}

			iser::CArchiveTag keyTag(key, QByteArrayLiteral("key"), iser::CArchiveTag::TT_LEAF);
			retVal = retVal && archive.BeginTag(keyTag);
			retVal = retVal && SerializeVariantValue(archive, value);
			retVal = retVal && archive.EndTag(keyTag);

			if (!retVal){
				break;
			}
		}

		if (wrapSubItem){
			retVal = retVal && archive.EndTag(subArrayTag);
		}
	}

	if (!isMultiTag){
		if (!tagName.isEmpty()){
			retVal = retVal && archive.EndTag(objectTag);
		}
	}
	else{
		retVal = retVal && archive.EndTag(arrayTag);
	}

	return retVal;
}


int CTreeItemModel::GetKeyRole(const QByteArray& key) const
{
	const QList<int> keys = m_roleNames.keys();
	for (int i : keys){
		if (m_roleNames[i] == key){
			return i;
		}
	}

	return -1;
}


bool CTreeItemModel::ParseRecursive(const QJsonObject& jsonObject, int index)
{
	bool retVal = true;

	for (QJsonObject::ConstIterator it = jsonObject.begin(); it != jsonObject.end(); ++it){
		const QByteArray key = it.key().toUtf8();
		const QJsonValue jsonValue = it.value();

		if (jsonValue.isArray()){
			CTreeItemModel* treeItemModel = AddTreeModel(key, index);
			if (treeItemModel == nullptr){
				return false;
			}

			treeItemModel->SetIsArray(true);

			const QJsonArray jsonArray = jsonValue.toArray();
			for (const QJsonValue& arrayValue : jsonArray){
				if (arrayValue.isObject()){
					const int childIndex = treeItemModel->InsertNewItem();
					retVal = retVal && treeItemModel->ParseRecursive(arrayValue.toObject(), childIndex);
				}
				else if (arrayValue.isArray()){
					// Unsupported yet
					retVal = false;
				}
				else{
					const int childIndex = treeItemModel->InsertNewItem();
					retVal = retVal && treeItemModel->SetData(QByteArray(), arrayValue.toVariant(), childIndex);
				}

				if (!retVal){
					break;
				}
			}
		}
		else if (jsonValue.isObject()){
			CTreeItemModel* treeItemModel = AddTreeModel(key, index);
			if (treeItemModel == nullptr){
				return false;
			}

			const QJsonObject valueObject = jsonValue.toObject();
			if (!valueObject.isEmpty()){
				retVal = retVal && treeItemModel->ParseRecursive(valueObject, 0);
			}
		}
		else{
			retVal = retVal && SetData(key, jsonValue.toVariant(), index);
		}

		if (!retVal){
			break;
		}
	}

	return retVal;
}


// private methods

#if QT_VERSION < 0x060000
inline int CTreeItemModel::GetVariantTypeId(const QVariant& value)
{
	return value.type();
}
#else
inline int CTreeItemModel::GetVariantTypeId(const QVariant& value)
{
	return value.typeId();
}
#endif


bool CTreeItemModel::SerializeVariantValue(iser::IArchive& archive, const QVariant& value)
{
	switch (GetVariantTypeId(value)){
	case QMetaType::Int: {
		qint32 intVal = value.toInt();
		return archive.Process(intVal);
	}
	case QMetaType::UInt: {
		quint32 intVal = value.toUInt();
		return archive.Process(intVal);
	}
	case QMetaType::Double: {
		double doubleVal = value.toDouble();
		return archive.Process(doubleVal);
	}
	case QMetaType::Float: {
		double floatVal = value.toFloat();
		return archive.Process(floatVal);
	}
	case QMetaType::LongLong: {
		qint64 intVal = value.toLongLong();
		return archive.Process(intVal);
	}
	case QMetaType::ULongLong: {
		quint64 intVal = value.toULongLong();
		return archive.Process(intVal);
	}
	case QMetaType::Bool: {
		bool boolVal = value.toBool();
		return archive.Process(boolVal);
	}
	case QMetaType::QString:
	case QMetaType::QByteArray: {
		QString strVal = value.toString();
		return archive.Process(strVal);
	}
	default: {
		QByteArray baVal = QByteArrayLiteral("null");
		return archive.Process(baVal);
	}
	}
}


} // namespace imtbase


