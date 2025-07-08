#include <imtbase/CItemModelBase.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>


namespace imtbase
{


// public methods

CItemModelBase::CItemModelBase(QObject* parent) : QObject(parent), m_owner(nullptr)
{
	QObject::connect(this, &CItemModelBase::modelChanged, this, &CItemModelBase::OnModelChanged);
}


void CItemModelBase::beginChanges()
{
	if (m_isTransaction){
		return;
	}

	m_changeList.clear();
	m_changeCount = 0;
	m_isTransaction = true;
}


void CItemModelBase::endChanges()
{
	if (!m_isTransaction){
		qWarning() << "No active transaction";
		return;
	}

	if (m_changeCount > 0){
		emit modelChanged(m_changeList);
		m_changeCount = 0;
	}

	m_isTransaction = false;
}


QString CItemModelBase::toJson() const
{
	QJsonObject obj;
	for (const QString& key : getProperties()){
		QString jsonKey = getJSONKeyForProperty(key);
		QVariant value = property(key.toUtf8());
		if (value.canConvert<CItemModelBase>()){
			CItemModelBase* objectPtr = value.value<CItemModelBase*>();
			obj[jsonKey] = QJsonDocument::fromJson(objectPtr->toJson().toUtf8()).object();
		}
		else{
			obj[jsonKey] = QJsonValue::fromVariant(value);
		}
	}

	return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}


bool CItemModelBase::createFromJson(const QString& json)
{
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
	if (err.error != QJsonParseError::NoError){
		qWarning() << "JSON parse error:" << err.errorString();
		return false;
	}

	QJsonObject obj = doc.object();
	for (auto it = obj.begin(); it != obj.end(); ++it){
		const QString& key = it.key();
		QVariant currentValue = property(key.toUtf8());

		if (it.value().isObject()){
			if (currentValue.canConvert<CItemModelBase>()){
				CItemModelBase* objectPtr = currentValue.value<CItemModelBase*>();
				if (objectPtr != nullptr){
					objectPtr->createFromJson(QString::fromUtf8(QJsonDocument(it.value().toObject()).toJson(QJsonDocument::Compact)));
				}
			}
			else{
				qWarning() << "Cannot assign object to property" << key;
			}
		}
		else{
			setProperty(key.toUtf8(), it.value().toVariant());
		}
	}

	emit finished();
	return true;
}


bool CItemModelBase::fromObject(const QJsonObject& jsonObject)
{
	QJsonDocument document;
	document.setObject(jsonObject);

	qDebug() << __func__ << document.toJson(QJsonDocument::Compact);

	return false;
}


QString CItemModelBase::toGraphQL() const
{
	QString result = "{";
	const QStringList keys = getProperties();
	for (int i = 0; i < keys.size(); ++i){
		QString key = keys[i];
		QVariant value = property(key.toUtf8());
		result += key + ":";
		if (value.typeId() == QMetaType::QString){
			result += "\"" + value.toString().replace("\"", "\\\"") + "\"";
		}
		else{
			result += value.toString();
		}

		if (i < keys.size() - 1){
			result += ",";
		}
	}
	result += "}";

	return result;
}


bool CItemModelBase::isEqualWithModel(CItemModelBase* otherObjectPtr) const
{
	if (otherObjectPtr == nullptr){
		return false;
	}

	const QStringList selfKeys = getProperties();
	const QStringList otherObjectKeys = otherObjectPtr->getProperties();

	if (selfKeys.size() != otherObjectKeys.size()){
		return false;
	}

	for (const QString& key : selfKeys){
		if (!otherObjectKeys.contains(key)){
			return false;
		}

		QVariant selfValue = property(key.toUtf8());
		QVariant otherObjectValue = otherObjectPtr->property(key.toUtf8());
		if (selfValue.typeId() != otherObjectValue.typeId()){
			return false;
		}

		if (selfValue.canConvert<CItemModelBase>()){
			CItemModelBase* selfBaseObjectPtr = selfValue.value<CItemModelBase*>();
			CItemModelBase* otherObjectBaseObjectPtr = otherObjectValue.value<CItemModelBase*>();
			if (selfBaseObjectPtr == nullptr || otherObjectBaseObjectPtr == nullptr){
				return false;
			}

			bool ok = selfBaseObjectPtr->isEqualWithModel(otherObjectBaseObjectPtr);
			if (!ok){
				return false;
			}
		}
		else if (selfValue != otherObjectValue){
			return false;
		}
	}

	return true;
}


CItemModelBase* CItemModelBase::copyMe()
{
	CItemModelBase* baseObjectPtr = new CItemModelBase(); // Leak ???
	if (baseObjectPtr != nullptr){
		baseObjectPtr->createFromJson(toJson());
		return baseObjectPtr;
	}

	return nullptr;
}


bool CItemModelBase::copyFrom(CItemModelBase* otherObjectPtr)
{
	if (otherObjectPtr != nullptr){
		return createFromJson(otherObjectPtr->toJson());
	}

	return false;
}


bool CItemModelBase::copyTo(CItemModelBase* otherObjectPtr)
{
	if (otherObjectPtr != nullptr){
		return otherObjectPtr->createFromJson(toJson());
	}

	return false;
}


QObject* CItemModelBase::CreateObject(const QString& key)
{
	return nullptr;
}


// private slots:

void CItemModelBase::OnInternalModelChanged()
{
	if (m_isTransaction){
		m_changeCount++;
		return;
	}

	emit modelChanged(m_changeList);
}


void CItemModelBase::OnModelChanged(const QVariantList& changes)
{
	if (m_owner == nullptr){
		return;
	}

	if (!m_owner->m_enableNotifications){
		return;
	}

	if (m_owner->m_isTransaction){
		m_owner->m_changeList.append(changes);
		m_owner->m_changeCount++;
	}
	else{
		emit m_owner->modelChanged(changes);
	}
}


// protected methods

QStringList CItemModelBase::getProperties() const
{
	QStringList list;
	const QMetaObject* meta = metaObject();
	for (int i = 0; i < meta->propertyCount(); i++){
		QMetaProperty prop = meta->property(i);
		QString name = QString::fromLatin1(prop.name());
		list.append(name);
	}

	return list;
}


QString CItemModelBase::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName.startsWith("m_") && propertyName.length() > 2) {
		QString transformed = propertyName.mid(2);
		transformed[0] = transformed[0].toUpper();
		return transformed;
	}

	return propertyName;
}


CItemModelBase* CItemModelBase::CreateItemModel(const QString& jsonKey)
{
	return nullptr;
}


// private methods

bool CItemModelBase::connectPropertyChangedToSlot(
			QObject* sender,
			const QString& propName,
			QObject* receiver,
			const char* slot)
{
	const QMetaObject* senderMeta = sender->metaObject();

	int propIndex = senderMeta->indexOfProperty(propName.toUtf8().constData());
	if (propIndex == -1) {
		qWarning() << "Property" << propName << "not found in" << sender;
		return false;
	}

	QMetaProperty prop = senderMeta->property(propIndex);
	if (!prop.hasNotifySignal()) {
		qWarning() << "Property" << propName << "has no notify signal";
		return false;
	}

	QMetaMethod signal = prop.notifySignal();

	int slotIndex = receiver->metaObject()->indexOfSlot(slot);
	if (slotIndex == -1) {
		qWarning() << "Slot" << slot << "not found in" << receiver;
		return false;
	}

	QMetaMethod slotMethod = receiver->metaObject()->method(slotIndex);

	return QObject::connect(
				sender, signal,
				receiver, slotMethod
				);
}


void CItemModelBase::connectProperties() 
{
	const QMetaObject* meta = metaObject();
	int count = meta->propertyCount();

	for (int i = 0; i < count; ++i) {
		QMetaProperty prop = meta->property(i);
		if (!prop.hasNotifySignal()){
			continue;
		}

		QString propName = QString::fromLatin1(prop.name());
		if (propName != "objectName" && !connectPropertyChangedToSlot(this, propName, this, SLOT(OnInternalModelChanged()))){
			qWarning() << "Failed to connect property:" << propName;
		}
	}
}


} // namespace imtbase


