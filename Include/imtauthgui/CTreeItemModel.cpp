#include <imtauthgui/CTreeItemModel.h>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauthgui
{


CTreeItemModel::CTreeItemModel(QObject *parent) : QAbstractListModel(parent)
{

}


int CTreeItemModel::addItem()
{
	m_items.append(new Item());
	return m_items.count() - 1;
}


void CTreeItemModel::addTreeModel(const QByteArray &key, int index)
{
	CTreeItemModel *treeItemModel = nullptr;
	if (index < 0 || index > m_items.count() - 1){
		return;
	}
	Item *item = m_items[index];
	QVariant v = QVariant::fromValue((CTreeItemModel *)(new CTreeItemModel(this)));
	item->SetValue(key, v);
}


bool CTreeItemModel::setData(const QByteArray &key, const QVariant &value, int index)
{
	CTreeItemModel *treeItemModel = nullptr;
	if (m_items.isEmpty() && index == 0){
		addItem();
	}
	if (index < 0 || index > m_items.count() - 1){
		return false;
	}
	QList<QByteArray> roles = m_roleNames.values();
	if (!roles.contains(key)){
		m_roleNames.insert(Qt::UserRole + 1 + m_roleNames.count(), key);
	}
	Item *item = m_items[index];
	item->SetValue(key, value);
	return true;
}


QVariant CTreeItemModel::getData(const QByteArray &key, int index) const
{
	if (index < 0 || index > m_items.count() - 1){
		return QVariant();
	}
	return m_items[index]->Value(key);
}


bool CTreeItemModel::isTreeModel(const QByteArray &key, int index)
{
	return getTreeItemModel(key,index) != nullptr;
}


CTreeItemModel *CTreeItemModel::getTreeItemModel(const QByteArray &key, int index)
{
	CTreeItemModel *treeItemModel = nullptr;
	QVariant v = getData(key,index);

	if (v.isValid()){
		treeItemModel = (CTreeItemModel*)v.value<CTreeItemModel *>();
	}
	return treeItemModel;
}


int CTreeItemModel::ItemsCount()
{
	return m_items.count();
}


int CTreeItemModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_items.count();
}


bool CTreeItemModel::SerializeRecursive(iser::IArchive &archive, const QByteArray& tagName)
{
	bool retVal = true;
	int countSize = m_items.count();
	iser::CArchiveTag arrayTag(tagName, "array items", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag subArrayTag("item", "array item", iser::CArchiveTag::TT_GROUP, &arrayTag);
	iser::CArchiveTag objectTag(tagName, "key", iser::CArchiveTag::TT_GROUP);
	if (countSize < 1){
		return false;
	}
	if (countSize == 1){
		retVal = retVal && archive.BeginTag(objectTag);
	}
	else if (countSize > 1){
		retVal = retVal && archive.BeginMultiTag(arrayTag, subArrayTag, countSize);
	}

	for (int i = 0; i < countSize; i++){
		Item *item = m_items[i];
		QList<QByteArray> keys;
		item->GetKeys(keys);
		if (countSize > 1){
			retVal = retVal && archive.BeginTag(subArrayTag);
		}
		for (const QByteArray& key: keys){
			CTreeItemModel *treeItemModel = nullptr;
			QVariant v = item->Value(key);
			if (v.isValid()){
				treeItemModel = (CTreeItemModel*)v.value<void *>();
			}
			if (treeItemModel!=nullptr){
				int itemsCount = treeItemModel->ItemsCount();
				if (itemsCount > 1){
					treeItemModel->SerializeRecursive(archive, key);
				}
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
		if (countSize > 1){
			retVal = retVal && archive.EndTag(subArrayTag);
		}

	}

	if (countSize == 1){
		retVal = retVal && archive.EndTag(objectTag);
	}
	else if (countSize > 1){
		retVal = retVal && archive.EndTag(arrayTag);
	}

}



// reimplemented (iser::ISerializable)

bool CTreeItemModel::Serialize(iser::IArchive &archive)
{
	bool retVal = true;
	int countSize = m_items.count();
	if (countSize < 1){
		return false;
	}
	retVal = SerializeRecursive(archive,"");
	return retVal;
}


QVariant CTreeItemModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	QByteArray key = m_roleNames.value(role);
	const QVariant v = this->getData(key, row);
	return v;
}

QHash<int, QByteArray> CTreeItemModel::roleNames() const
{
	return m_roleNames;
}


} // namespace imtauthgui
