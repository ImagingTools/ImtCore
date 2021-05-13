#include <imtbase/CTreeItemModel.h>


// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>


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

}


const QString& CTreeItemModel::state() const
{
	return m_state;
}


void CTreeItemModel::setState(const QString &newState)
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

	return m_items.count() - 1;
}


CTreeItemModel* CTreeItemModel::AddTreeModel(const QByteArray &key, int index)
{
	if (m_items.isEmpty() && index == 0){
		InsertNewItem();
	}

	if (index < 0 || index > m_items.count() - 1){
		return nullptr;
	}

	Item *item = m_items[index];
	CTreeItemModel* retVal = new CTreeItemModel(this);

//	int level = 0;
//	CTreeItemModel* parentModel = this;
//	while (parentModel != nullptr){
//		parentModel = dynamic_cast<CTreeItemModel*>(parentModel->parent());
//		if (parentModel != nullptr){
//			level++;
//		}
//	}

	QVariant v = QVariant::fromValue(retVal);

	item->SetValue(key, v);

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
	changeSet.SetChangeInfo("curVal", item->Value(key));
	changeSet.SetChangeInfo("newVal", value);
	changeSet.SetChangeInfo("key", key);
	changeSet.SetChangeInfo("index", index);

	BeginChanges(changeSet);

	item->SetValue(key, value);

	EndChanges(changeSet);

	return true;
}


QVariant CTreeItemModel::GetData(const QByteArray &key, int index) const
{
	if (index < 0 || index > m_items.count() - 1){
		return QVariant();
	}

	return m_items[index]->Value(key);
}


bool CTreeItemModel::IsTreeModel(const QByteArray &key, int index) const
{
	return GetTreeItemModel(key,index) != nullptr;
}


CTreeItemModel* CTreeItemModel::GetTreeItemModel(const QByteArray &key, int index) const
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


// reimplemented (QAbstractListModel)

int CTreeItemModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_items.count();
}


QVariant CTreeItemModel::data(const QModelIndex& index, int role) const
{
	QByteArray key = m_roleNames.value(role);

	int row = index.row();

	return this->GetData(key, row);
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
	if (countSize < 1){
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
	if (countSize < 1){
		return false;
	}
	bool isMultiTag = countSize > 1 || !tagName.isEmpty();
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
		if (isMultiTag == true){
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
				else{
					QString strVal = value.toString();
					retVal = retVal && archive.Process(strVal);
				}
				retVal = retVal && archive.EndTag(keyTag);
			}

		}
		if (isMultiTag == true){
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


