#include <imtbase/CTreeItemModel.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CJsonMemWriteArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/TSmartPtr.h>
#include <imod/CModelUpdateBridge.h>


namespace imtbase
{


CTreeItemModel::CTreeItemModel(QObject *parent)
	:QAbstractListModel(parent),
	m_isArray(false)
{
	imtbase::CTreeItemModel* parentModel = dynamic_cast<imtbase::CTreeItemModel*>(parent);
	if (parentModel != nullptr){
		SetSlavePtr(parentModel);
	}
}

CTreeItemModel::~CTreeItemModel()
{
	Clear();
}


const QString& CTreeItemModel::State() const
{
	return m_state;
}


void CTreeItemModel::SetState(const QString &newState)
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


bool CTreeItemModel::Copy(CTreeItemModel* object)
{
	bool result = CopyFrom(*object);

	return result;
}


void CTreeItemModel::InsertNewItemWithParameters(int index, const QVariantMap &map)
{
	if (index < 0 || index > m_items.count()){
		return;
	}

	beginInsertRows(QModelIndex(), index, index);

	m_items.insert(index, new Item());

	for(auto value = map.cbegin(); value != map.cend(); ++ value){
		SetData(value.key().toUtf8(), *value, index);
	}

	endInsertRows();
}


int CTreeItemModel::InsertNewItem()
{
	m_items.append(new Item());

	if(m_items.count() > 1){
		m_isArray = true;
	}

	int index = m_items.count() - 1;

	beginInsertRows(QModelIndex(), index, index);
	endInsertRows();

	return index;
}


int CTreeItemModel::RemoveItem(int index, const ChangeInfoMap& /*infoMap*/)
{
	if (index < 0 || index > m_items.count() - 1){
		return false;
	}

	beginRemoveRows(QModelIndex(), index, index);

	IChangeable::ChangeSet changeSet = IChangeable::GetAnyChange();
	changeSet.SetChangeInfo("operation", "remove item");
	changeSet.SetChangeInfo("index", index);

	BeginChanges(changeSet);

	Item* item = m_items.takeAt(index);
	QList<QByteArray> keys;

	delete item;

	if(m_items.isEmpty()){
		m_isArray = false;
	}

	EndChanges(changeSet);

	endRemoveRows();

	return true;
}


imtbase::CTreeItemModel* CTreeItemModel::AddTreeModel(const QByteArray &key, int index)
{
	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		return nullptr;
	}

	CTreeItemModel* retVal = GetTreeItemModel(key, index);

	if (retVal == nullptr){
		retVal = new CTreeItemModel(this);

		QVariant v = QVariant::fromValue(retVal);

		SetData(key, v, index);
	}
	else{
		istd::CChangeNotifier changeNotifier(this);

		retVal->Clear();
	}

	return retVal;
}


bool CTreeItemModel::SetExternTreeModel(const QByteArray &key, CTreeItemModel *externTreeModel, int index)
{
	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		return false;
	}

	if (externTreeModel != nullptr){
		externTreeModel->setParent(this);
		externTreeModel->SetSlavePtr(this);
	}

	QVariant v = QVariant::fromValue(externTreeModel);
	SetData(key, v, index);

	return true;
}

bool CTreeItemModel::CopyItemDataFromModel(int index, CTreeItemModel *externTreeModel, int externIndex)
{
	const CTreeItemModel* constExternTreeModel = externTreeModel;
	return CopyItemDataFromModel(index, constExternTreeModel, externIndex);
}

bool CTreeItemModel::CopyItemDataFromModel(int index, const CTreeItemModel *externTreeModel, int externIndex)
{
	bool retVal = false;
	QList<QByteArray> keys;
	externTreeModel->GetKeys(keys, externIndex);
	for (QByteArray key : keys){
		QVariant value = externTreeModel->GetData(key, externIndex);

		CTreeItemModel* treeItemModelPtr = nullptr;
		if (value.isValid()){
			treeItemModelPtr = value.value<CTreeItemModel*>();
		}
		if (treeItemModelPtr != nullptr){
			CTreeItemModel* childModelPtr = AddTreeModel(key, index);

			int itemsCount = treeItemModelPtr->GetItemsCount();

			for (int i = 0; i < itemsCount; i++){
				int childIndex = childModelPtr->InsertNewItem();
				retVal = childModelPtr->CopyItemDataFromModel(childIndex, treeItemModelPtr, i);
				if (retVal == false){
					break;
				}
			}
		}
		else{
			retVal = SetData(key, value, index);
		}
		if (retVal == false){
			break;
		}
	}
	return retVal;
}


bool CTreeItemModel::CopyItemDataToModel(int index, CTreeItemModel *externTreeModel, int externIndex) const
{
	bool retVal = false;
	QList<QByteArray> keys;
	this->GetKeys(keys, index);

	for (QByteArray key : keys){
		QVariant value = GetData(key, index);
		retVal = externTreeModel->SetData(key, value, externIndex);

		if (retVal == false){
			break;
		}
	}

	return retVal;
}


bool CTreeItemModel::SetData(const QByteArray& key, const QVariant& value, int index, const istd::IChangeable::ChangeInfoMap& /*infoMap*/)
{
	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		return false;
	}

	QList<QByteArray> roles = m_roleNames.values();
	if (!roles.contains(key)){
		m_roleNames.insert(Qt::UserRole + 1 + m_roleNames.count(), key);
	}

	Item* item = m_items[index];
	IChangeable::ChangeSet changeSet = IChangeable::GetAnyChange();

	changeSet.SetChangeInfo("operation", "set");
	changeSet.SetChangeInfo("curVal", item->Value(key));
	changeSet.SetChangeInfo("newVal", value);
	changeSet.SetChangeInfo("key", key);
	changeSet.SetChangeInfo("index", index);

	BeginChanges(changeSet);

	item->SetValue(key, value);

	EndChanges(changeSet);

	int keyRole = -1;
	QList<int> keys = m_roleNames.keys();
	for (int i : keys){
		if (m_roleNames[i] == key){
			keyRole = i;
			break;
		}
	}
	if (keyRole > -1){
		QModelIndex topLeft = QAbstractListModel::index(index - 1);
		QModelIndex bottomRight = QAbstractListModel::index(index);

		QVector<int> keyRoles;
		keyRoles.append(keyRole);

		emit dataChanged(topLeft, bottomRight, keyRoles);
	}

	return true;
}


bool CTreeItemModel::RemoveData(const QByteArray& key, int index, const ChangeInfoMap& /*infoMap*/)
{
	if (index < 0 || index > m_items.count() - 1){
		return false;
	}

	Item* item = m_items[index];
	IChangeable::ChangeSet changeSet = IChangeable::GetAnyChange();

	changeSet.SetChangeInfo("operation", "remove");
	changeSet.SetChangeInfo("curVal", item->Value(key));
	changeSet.SetChangeInfo("key", key);
	changeSet.SetChangeInfo("index", index);

	BeginChanges(changeSet);

	item->RemoveValue(key);

	EndChanges(changeSet);

	int keyRole = -1;
	QList<int> keys = m_roleNames.keys();
	for (int i : keys){
		if (m_roleNames[i] == key){
			keyRole = i;
			break;
		}
	}
	if (keyRole > -1){
		QModelIndex topLeft = QAbstractListModel::index(index);
		QVector<int> roles;
		roles.append(keyRole);
		emit dataChanged(topLeft,topLeft,roles);
	}

	return true;
}


QVariant CTreeItemModel::GetData(const QByteArray &key, int index) const
{
	if (index < 0 || index > m_items.count() - 1){
		return QVariant();
	}

	return m_items[index]->Value(key);
}


imtbase::CTreeItemModel *CTreeItemModel::GetParent() const
{
	return dynamic_cast<CTreeItemModel*>(this->parent());
}


bool CTreeItemModel::IsTreeModel(const QByteArray &key, int index) const
{
	return GetTreeItemModel(key,index) != nullptr;
}


bool CTreeItemModel::ContainsKey(const QByteArray &key, int index) const
{
	bool retVal = false;

	if (index >= 0 && index < m_items.count()){
		retVal = m_items[index]->ContainsKey(key);
	}

    return retVal;
}


bool CTreeItemModel::IsValidData(const QByteArray &key, int index) const
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


CTreeItemModel *CTreeItemModel::GetModelFromItem(int itemIndex) const
{
	int itemCount = GetItemsCount();

	if (itemCount <= itemIndex){
		return nullptr;
	}

	CTreeItemModel* modelPtr = new CTreeItemModel();

//	CopyItemDataToModel(itemIndex, modelPtr);
	modelPtr->CopyItemDataFromModel(0, this, itemIndex);

	return modelPtr;
}


int CTreeItemModel::GetItemsCount() const
{
	return m_items.count();
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
	for (const QByteArray& key : keys){
		keys2.append(key);
	}

	return keys2;
}


void CTreeItemModel::Clear()
{
	qDeleteAll(m_items);
	m_items.clear();
}


bool CTreeItemModel::IsArray()
{
	return m_isArray;
}


void CTreeItemModel::SetIsArray(const bool &isArray)
{
	m_isArray = isArray;
}


bool CTreeItemModel::CreateFromJson(const QByteArray &jsonContent)
{
	Clear();
	QJsonParseError error;

	QJsonDocument document = QJsonDocument::fromJson(jsonContent, &error);
	if (error.error != QJsonParseError::NoError){
		qCritical()  << "Error during parsing JSON document:" << error.errorString();
		return false;
	}

	bool retVal = true;
	if (document.isArray()){
		QJsonArray jsonArray = document.array();
		int index;
		for (auto v : jsonArray){
			index = InsertNewItem();
			QJsonObject element = v.toObject();
			retVal = retVal && ParseRecursive(element, index);
		}
	}
	else{
		InsertNewItem();
		retVal = ParseRecursive(document.object(), 0);
	}

	return retVal;
}


void CTreeItemModel::SetQueryParam(const QByteArray &key, const QByteArray &value)
{
	m_queryParams.insert(key, value);
}


QByteArray CTreeItemModel::GetQueryParam(const QByteArray &key)
{
	return m_queryParams.value(key);
}


QByteArray CTreeItemModel::TakeQueryParam(const QByteArray &key)
{
	return m_queryParams.take(key);
}


QMap<QByteArray, QByteArray> &CTreeItemModel::GetQueryParams()
{
	return m_queryParams;
}


void CTreeItemModel::ClearQueryParams(const QByteArray& /*key*/)
{
	m_queryParams.clear();
}


void CTreeItemModel::Refresh()
{
	beginResetModel();
	endResetModel();
}


QString CTreeItemModel::toJSON()
{
	QByteArray representationData;

	{
		iser::CJsonMemWriteArchive archive(representationData);

		Serialize(archive);
	}

	return QString(representationData);
}


// reimplemented (QAbstractListModel)

int CTreeItemModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_items.count();
}


QVariant CTreeItemModel::data(const QModelIndex& index, int role) const
{
	if (m_roleNames.contains(role) == false){
		return QVariant();
	}

	QByteArray key = m_roleNames.value(role);

	int row = index.row();

	return this->GetData(key, row);
}

bool CTreeItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
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


// reimplemented (iser::ISerializable)

bool CTreeItemModel::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(!archive.IsStoring() ? this : nullptr);

	if (!archive.IsStoring()){
		Clear();
	}

	return SerializeRecursive(archive,"");
}


// reimplemented (istd::IChangeable)

int CTreeItemModel::GetSupportedOperations() const
{
	return SO_COPY;
}


bool CTreeItemModel::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const imtbase::CTreeItemModel* sourcePtr = dynamic_cast<const imtbase::CTreeItemModel*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		Clear();

		for (int i = 0; i < sourcePtr->m_items.count(); i++){
			Item* item = new Item();
			Item* sourceItem = sourcePtr->m_items[i];

			QList<QByteArray> keys;
			sourceItem->GetKeys(keys);

			for (const QByteArray& key : keys){
				QVariant value = sourceItem->Value(key);

				CTreeItemModel* treeItemModelPtr = nullptr;
				if (value.isValid()){
					treeItemModelPtr = value.value<CTreeItemModel*>();
				}

				if (treeItemModelPtr != nullptr){
					CTreeItemModel* subModelPtr = new CTreeItemModel();
					subModelPtr->CopyFrom(*treeItemModelPtr);

					subModelPtr->setParent(this);
					subModelPtr->SetSlavePtr(this);

					item->SetValue(key, QVariant::fromValue(subModelPtr));
				}
				else{
					item->SetValue(key, value);
				}
			}

			m_items.append(item);
		}

		m_roleNames = sourcePtr->m_roleNames;
		m_queryParams = sourcePtr->m_queryParams;
		m_isArray = sourcePtr->m_isArray;
		m_state = sourcePtr->m_state;

		return true;
	}

	return false;
}


istd::IChangeable* CTreeItemModel::CloneMe(CompatibilityMode /*mode*/) const
{
	return NULL;
}



// protected methods

bool CTreeItemModel::SerializeRecursive(iser::IArchive &archive, const QByteArray& tagName)
{
	bool retVal = true;
	int countSize = m_items.count();
	iser::CArchiveTag arrayTag(tagName, "array items", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag subArrayTag("Item", "array item", iser::CArchiveTag::TT_GROUP, &arrayTag);
	iser::CArchiveTag objectTag(tagName, "key", iser::CArchiveTag::TT_GROUP);
	bool isMultiTag = false;

	if (countSize < 1 && m_isArray == false){
		return false;
	}

	if (m_isArray || countSize > 1){
		isMultiTag = true;
	}

	if (isMultiTag == false){
		if (!tagName.isEmpty()){
			retVal = retVal && archive.BeginTag(objectTag);
		}
	}
	else{
		retVal = retVal && archive.BeginMultiTag(arrayTag, subArrayTag, countSize);
	}

	for (int i = 0; i < countSize; i++){
		Item *item = m_items[i];
		QList<QByteArray> keys;
		item->GetKeys(keys);
		if (isMultiTag == true && !keys.isEmpty() && keys[0] != ""){
			retVal = retVal && archive.BeginTag(subArrayTag);
		}
		for (const QByteArray& key: keys){
			CTreeItemModel* treeItemModelPtr = nullptr;
			QVariant v = item->Value(key);
			if (v.isValid()){
				treeItemModelPtr = v.value<CTreeItemModel*>();
			}
			if (treeItemModelPtr != nullptr){
				treeItemModelPtr->SerializeRecursive(archive, key);
			}
			else{
				iser::CArchiveTag keyTag(key, "key", iser::CArchiveTag::TT_LEAF);
				retVal = retVal && archive.BeginTag(keyTag);
				QVariant value = item->Value(key);
				if (value.type() == QVariant::Int){
					qint32 intVal = value.toInt();
					retVal = retVal && archive.Process(intVal);
				}
				else if (value.type() == QVariant::UInt){
					quint32 intVal = value.toUInt();
					retVal = retVal && archive.Process(intVal);
				}
				else if (value.type() == QVariant::Double){
					double doubleVal = value.toDouble();
					retVal = retVal && archive.Process(doubleVal);
				}
				else if (value.type() == QVariant::LongLong){
					qint64 intVal = value.toLongLong();
					retVal = retVal && archive.Process(intVal);
				}
				else if (value.type() == QVariant::ULongLong){
					quint64 intVal = value.toULongLong();
					retVal = retVal && archive.Process(intVal);
				}
				else if (value.type() == QVariant::Bool){
					bool boolVal = value.toBool();
					retVal = retVal && archive.Process(boolVal);
				}
				else if (value.type() == QVariant::String || value.type() == QVariant::ByteArray){
					QString strVal = value.toString();
					retVal = retVal && archive.Process(strVal);
				}
				else {
					QByteArray baVal = "null";
					retVal = retVal && archive.Process(baVal);
				}
				retVal = retVal && archive.EndTag(keyTag);
			}
		}
		if (isMultiTag == true && !keys.isEmpty() && keys[0] != ""){
			retVal = retVal && archive.EndTag(subArrayTag);
		}
	}

	if (isMultiTag == false){
		if (!tagName.isEmpty()){
			retVal = retVal && archive.EndTag(objectTag);
		}
	}
	else{
		retVal = retVal && archive.EndTag(arrayTag);
	}

	return retVal;
}


bool CTreeItemModel::ParseRecursive(const QJsonObject& jsonObject, int index)
{
	bool retVal = true;
	QJsonObject::ConstIterator objectIterator = jsonObject.begin();
	while (objectIterator != jsonObject.end()) {
		QJsonValue jsonValue = objectIterator.value();
		if (jsonValue.isArray()){
			CTreeItemModel* treeItemModel = AddTreeModel(objectIterator.key().toUtf8(), index);
			treeItemModel->SetIsArray(true);
			QJsonArray jsonArrary = jsonValue.toArray();
			QJsonArray::ConstIterator arrayIterator = jsonArrary.begin();
			while (arrayIterator != jsonArrary.end()) {
				jsonValue = *arrayIterator;
				if(jsonValue.isObject()){
					treeItemModel->InsertNewItem();
					treeItemModel->ParseRecursive(jsonValue.toObject(), treeItemModel->m_items.count() - 1);
				}
				arrayIterator++;
			}
		}
		else if(jsonValue.isObject()){
			CTreeItemModel* treeItemModel = AddTreeModel(objectIterator.key().toUtf8(), index);
			treeItemModel->InsertNewItem();
			treeItemModel->ParseRecursive(jsonValue.toObject(), treeItemModel->m_items.count() - 1);
		}
		else{
			SetData(objectIterator.key().toUtf8(),objectIterator.value().toVariant(), index);
		}
		objectIterator++;
	}

	return retVal;
}


void CTreeItemModel::subModelChanged(const CTreeItemModel *model, ChangeSet &changeSet)
{
	for (int i = 0; i < m_items.count(); i++){
		Item* item = m_items[i];
		QList<QByteArray> keys;
		item->GetKeys(keys);
		for (int j = 0; j < keys.count(); j++){
			if (item->Value(keys[j]).value<CTreeItemModel*>() == model){
				int level = changeSet.GetChangeInfo("level").toInt();
				level++;
				changeSet.SetChangeInfo("level", level);
				changeSet.SetChangeInfo("key" + QByteArray::number(level), keys[j]);
				changeSet.SetChangeInfo("index" + QByteArray::number(level), i);
			}
		}
	}
	CTreeItemModel* parentModel = dynamic_cast<CTreeItemModel*>(parent());
	if (parentModel != nullptr){
		parentModel->subModelChanged(model, changeSet);
	}
	else{
		istd::CChangeNotifier changeNotifier(this, &changeSet);
	}
}


void CTreeItemModel::OnEndChanges(const ChangeSet& changeSet)
{
	BaseClass::OnEndChanges(changeSet);

	emit modelChanged(changeSet);
}


void CTreeItemModel::OnBeginChanges()
{
	BaseClass::OnBeginChanges();
}


} // namespace imtbase


