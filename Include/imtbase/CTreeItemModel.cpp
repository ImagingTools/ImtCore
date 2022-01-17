#include <imtbase/CTreeItemModel.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CJsonStringWriteArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{


CTreeItemModel::CTreeItemModel(QObject *parent)
	:QAbstractListModel(parent),
	m_parentUpdateBridge(dynamic_cast<CTreeItemModel*>(parent))
{
	CTreeItemModel* parentModel = dynamic_cast<CTreeItemModel*>(parent);
	if (parentModel != nullptr){
		this->AttachObserver(&m_parentUpdateBridge);
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


int CTreeItemModel::RemoveItem(int index, const ChangeInfoMap &infoMap)
{
	if (index < 0 || index > m_items.count() - 1){
		return false;
	}

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

//	beginRemoveRows(QModelIndex(this), index, index);
//	endRemoveRows();

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

	Item *item = m_items[index];

	CTreeItemModel* retVal = new CTreeItemModel(this);

	QVariant v = QVariant::fromValue(retVal);

	SetData(key, v, index);

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

	Item *item = m_items[index];
	if (externTreeModel != nullptr){
		externTreeModel->setParent(this);
	}
	QVariant v = QVariant::fromValue(externTreeModel);
	SetData(key, v, index);

	return true;
}

bool CTreeItemModel::CopyItemDataFromModel(int index, CTreeItemModel *externTreeModel, int externIndex)
{
	bool retVal = false;
	QList<QByteArray> keys;
	externTreeModel->GetKeys(keys, externIndex);
	for (QByteArray key : keys){
		QVariant value = externTreeModel->GetData(key, externIndex);
		retVal = SetData(key, value, index);
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
		QVector<int> roles;
		roles.append(keyRole);
		emit dataChanged(topLeft,bottomRight,roles);
	}

	return true;
}

bool CTreeItemModel::RemoveData(const QByteArray &key, int index, const ChangeInfoMap &infoMap)
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


imtbase::CTreeItemModel* CTreeItemModel::GetTreeItemModel(const QByteArray &key, int index) const
{
	QVariant data = GetData(key, index);
	if (data.isValid()){
		return data.value<CTreeItemModel*>();
	}

	return nullptr;
}


int CTreeItemModel::GetItemsCount() const
{
	return m_items.count();
}


void CTreeItemModel::GetKeys(QList<QByteArray>& keys, int index)
{
	if (index >= 0 && index < m_items.count()){
		m_items[index]->GetKeys(keys);
	}
}


QList<QByteArray> CTreeItemModel::GetKeys(int index)
{
	QList<QByteArray> keys;
	GetKeys(keys, index);
	return keys;
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


bool CTreeItemModel::Parse(const QByteArray &data)
{
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(data, &error);
	if (error.error != QJsonParseError::NoError){
		qCritical()  << "Error for parsing json document:" << error.errorString();
		return false;
	}
	Clear();
	InsertNewItem();
	bool retVal = ParseRecursive(document.object(), 0);
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


void CTreeItemModel::ClearQueryParams(const QByteArray &key)
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
		iser::CJsonStringWriteArchive archive(representationData);
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


// pseudo-reimplemented (istd::IChangeable)

void CTreeItemModel::BeginChanges(const ChangeSet &changeSet)
{
	NotifyBeforeChange(changeSet,false);
}


void CTreeItemModel::EndChanges(const ChangeSet &changeSet)
{
	NotifyAfterChange(changeSet);
}


void CTreeItemModel::BeginChangeGroup(const ChangeSet &changeSet)
{
	NotifyBeforeChange(changeSet,true);
}

void CTreeItemModel::EndChangeGroup(const ChangeSet &changeSet)
{
	NotifyAfterChange(changeSet);
}


// reimplemented (iser::ISerializable)

bool CTreeItemModel::Serialize(iser::IArchive &archive)
{
	int countSize = m_items.count();
	if (!archive.IsStoring()){
		Clear();
	}
	else if (countSize < 1){
		return false;
	}

	return SerializeRecursive(archive,"");
}


// protected methods

bool CTreeItemModel::SerializeRecursive(iser::IArchive &archive, const QByteArray& tagName)
{
	bool retVal = true;
	int countSize = m_items.count();
	iser::CArchiveTag arrayTag(tagName, "array items", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag subArrayTag(tagName, "array item", iser::CArchiveTag::TT_GROUP, &arrayTag);
	iser::CArchiveTag objectTag(tagName, "key", iser::CArchiveTag::TT_GROUP);
	bool isMultiTag = false;

	if (countSize < 1){
		return false;
	}
	isMultiTag = countSize > 1 || m_isArray == true;
	if (isMultiTag == false){
		retVal = retVal && archive.BeginTag(objectTag);
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
				else if (value.type() == QVariant::String){
					QString strVal = value.toString();
					retVal = retVal && archive.Process(strVal);
				}
				else if (value.type() == QVariant::ByteArray){
					QString strVal = value.toByteArray();
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
		retVal = retVal && archive.EndTag(objectTag);
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
			CTreeItemModel* treeItemModel = AddTreeModel(objectIterator.key().toUtf8());
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
			CTreeItemModel* treeItemModel = AddTreeModel(objectIterator.key().toUtf8());
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


void CTreeItemModel::OnBeginGlobalChanges()
{
	this->OnBeginChanges();
}


void CTreeItemModel::OnEndGlobalChanges(const ChangeSet &changeSet)
{
	this->OnEndChanges(changeSet);
}


} // namespace imtbase


