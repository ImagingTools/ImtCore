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

	beginInsertRows(QModelIndex(), index, index);

	m_items.append(new Item());

	if(m_items.count() > 1){
		m_isArray = true;
	}

	endInsertRows();

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

	beginInsertRows(QModelIndex(), index, index);

	m_items.insert(index, new Item());

	if(m_items.count() > 1){
		m_isArray = true;
	}

	endInsertRows();

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

	beginRemoveRows(QModelIndex(), index, index);

	Item* item = m_items.takeAt(index);
	delete item;

	endRemoveRows();

	if(m_items.isEmpty()){
		m_isArray = false;
	}

	OnModelChanged();

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

//	beginMoveRows(QModelIndex(), index1, index1, QModelIndex(), index2);

	beginResetModel();
	m_items.swapItemsAt(index1, index2);
//	endMoveRows();
	endResetModel();

	return true;
}


imtbase::CTreeItemModel* CTreeItemModel::AddTreeModel(const QByteArray& key, int index)
{
	BeginChanges();

	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		EndChanges();

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

	EndChanges();

	return retVal;
}


bool CTreeItemModel::SetExternTreeModel(const QByteArray& key, CTreeItemModel* externTreeModel, int index)
{
	BeginChanges();

	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		EndChanges();

		return false;
	}

	if (externTreeModel != nullptr){
		externTreeModel->setParent(this);
	}

	QVariant v = QVariant::fromValue(externTreeModel);
	SetData(key, v, index);

	EndChanges();

	return true;
}


bool CTreeItemModel::CopyItemDataFromModel(int index, CTreeItemModel* externTreeModel, int externIndex)
{
	const CTreeItemModel* constExternTreeModel = externTreeModel;

	return CopyItemDataFromModel(index, constExternTreeModel, externIndex);
}


bool CTreeItemModel::CopyItemDataFromModel(int index, const CTreeItemModel* externTreeModel, int externIndex)
{
	BeginChanges();

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

	EndChanges();

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

		Q_EMIT dataChanged(QAbstractListModel::index(index), QAbstractListModel::index(index), roles);
	}

	OnModelChanged();

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
	beginResetModel();

	qDeleteAll(m_items);
	m_items.clear();

	endResetModel();

	OnModelChanged();
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
	BeginChanges();

	Clear();
	QJsonParseError error;

	QJsonDocument document = QJsonDocument::fromJson(jsonContent, &error);
	if (error.error != QJsonParseError::NoError){
		qCritical() << "Error during parsing JSON document:" << error.errorString() << "content:" << jsonContent;
		EndChanges();

		return false;
	}

	bool retVal = true;
	if (document.isArray()){
		const QJsonArray jsonArray = document.array();
		int index;
		for (const auto& v : jsonArray){
			index = InsertNewItem();
			QJsonObject element = v.toObject();
			retVal = retVal && ParseRecursive(element, index);
		}
	}
	else{
		retVal = ParseRecursive(document.object(), 0);
	}

	EndChanges();

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

	{
		iser::CJsonMemWriteArchive archive(nullptr, false);

		if (SerializeModel(archive)){
			representationData = archive.GetData();
		}
		else{
			Q_ASSERT(false);
		}
	}

	if (representationData.length() > 2 && representationData[1] == '['){
		representationData = representationData.mid(1, representationData.length() - 2);
	}

	return QString(representationData);
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
	bool isMultiTag = false;

	QList<QByteArray> keys = m_roleNames.values();
	if (keys.size() == 1 && keys[0].isEmpty()){
		subArrayTag = iser::CArchiveTag(QByteArrayLiteral("Item"), QByteArrayLiteral("array item"), iser::CArchiveTag::TT_LEAF, &arrayTag);
	}

	if (m_isArray || countSize > 1){
		isMultiTag = true;
	}

	if (!isMultiTag){
		if (!tagName.isEmpty()){
			retVal = retVal && archive.BeginTag(objectTag);
		}
	}
	else{
		retVal = retVal && archive.BeginMultiTag(arrayTag, subArrayTag, countSize);
	}

	for (int i = 0; i < countSize; i++){
		Item* item = m_items[i];
		QList<QByteArray> itemKeys;
		item->GetKeys(itemKeys);
		if (isMultiTag && !itemKeys.isEmpty() && !itemKeys[0].isEmpty()){
			retVal = retVal && archive.BeginTag(subArrayTag);
		}
		for (const QByteArray& key: std::as_const(itemKeys)){
			CTreeItemModel* treeItemModelPtr = nullptr;
			QVariant v = item->Value(key);
			if (v.isValid()){
				treeItemModelPtr = v.value<CTreeItemModel*>();
			}
			if (treeItemModelPtr != nullptr){
				treeItemModelPtr->SerializeRecursive(archive, key);
			}
			else{
				iser::CArchiveTag keyTag(key, QByteArrayLiteral("key"), iser::CArchiveTag::TT_LEAF);
				retVal = retVal && archive.BeginTag(keyTag);
				QVariant value = item->Value(key);
#if QT_VERSION < 0x060000
				if (value.type() == QMetaType::Int){
#else
				if (value.typeId() == QMetaType::Int){
#endif
					qint32 intVal = value.toInt();
					retVal = retVal && archive.Process(intVal);
				}
#if QT_VERSION < 0x060000
				else if (value.type() == QMetaType::UInt){
#else
				else if (value.typeId() == QMetaType::UInt){
#endif
					quint32 intVal = value.toUInt();
					retVal = retVal && archive.Process(intVal);
				}
#if QT_VERSION < 0x060000
				else if (value.type() == QMetaType::Double){
#else
				else if (value.typeId() == QMetaType::Double){
#endif
					double doubleVal = value.toDouble();
					retVal = retVal && archive.Process(doubleVal);
				}
#if QT_VERSION < 0x060000
				else if (value.type() == QMetaType::Float){
#else
				else if (value.typeId() == QMetaType::Float){
#endif
					double floatVal = value.toFloat();
					retVal = retVal && archive.Process(floatVal);
				}
#if QT_VERSION < 0x060000
				else if (value.type() == QMetaType::LongLong){
#else
				else if (value.typeId() == QMetaType::LongLong){
#endif
					qint64 intVal = value.toLongLong();
					retVal = retVal && archive.Process(intVal);
				}
#if QT_VERSION < 0x060000
				else if (value.type() == QMetaType::ULongLong){
#else
				else if (value.typeId() == QMetaType::ULongLong){
#endif
					quint64 intVal = value.toULongLong();
					retVal = retVal && archive.Process(intVal);
				}
#if QT_VERSION < 0x060000
				else if (value.type() == QMetaType::Bool){
#else
				else if (value.typeId() == QMetaType::Bool){
#endif
					bool boolVal = value.toBool();
					retVal = retVal && archive.Process(boolVal);
				}
#if QT_VERSION < 0x060000
				else if (value.type() == QMetaType::QString || value.type() == QMetaType::QByteArray){
#else
				else if (value.typeId() == QMetaType::QString || value.typeId() == QMetaType::QByteArray){
#endif
					QString strVal = value.toString();
					retVal = retVal && archive.Process(strVal);
				}
				else {
					QByteArray baVal = QByteArrayLiteral("null");
					retVal = retVal && archive.Process(baVal);
				}
				retVal = retVal && archive.EndTag(keyTag);
			}
		}
		if (isMultiTag && !itemKeys.isEmpty() && !itemKeys[0].isEmpty()){
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
	QJsonObject::ConstIterator objectIterator = jsonObject.begin();
	while (objectIterator != jsonObject.end()){
		QJsonValue jsonValue = objectIterator.value();
		if (jsonValue.isArray()){
			CTreeItemModel* treeItemModel = AddTreeModel(objectIterator.key().toUtf8(), index);
			treeItemModel->SetIsArray(true);
			QJsonArray jsonArrary = jsonValue.toArray();
			QJsonArray::ConstIterator arrayIterator = jsonArrary.begin();
			while (arrayIterator != jsonArrary.end()){
				jsonValue = *arrayIterator;
				if(jsonValue.isObject()){
					treeItemModel->InsertNewItem();
					treeItemModel->ParseRecursive(jsonValue.toObject(), treeItemModel->m_items.size() - 1);
				}
				else if(jsonValue.isArray()){
				}
				else{
					int i = treeItemModel->InsertNewItem();
					treeItemModel->SetData(QByteArray(), jsonValue.toVariant(), i);
				}

				arrayIterator++;
			}
		}
		else if(jsonValue.isObject()){
			CTreeItemModel* treeItemModel = AddTreeModel(objectIterator.key().toUtf8(), index);

			QJsonObject valueObject = jsonValue.toObject();
			if (!valueObject.isEmpty()){
				treeItemModel->ParseRecursive(valueObject, 0);
			}
		}
		else{
			SetData(objectIterator.key().toUtf8(),objectIterator.value().toVariant(), index);
		}
		objectIterator++;
	}

	return retVal;
}


} // namespace imtbase


