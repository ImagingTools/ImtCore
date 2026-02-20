// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CItemModelBase.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>


namespace imtbase
{


// public methods

CItemModelBase::CItemModelBase(QObject* parent)
	:QObject(parent),
	m_owner(nullptr)
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
	qDebug() << "toGraphQL" << this;
	QString result = "{";
	const QStringList keys = getProperties();
	for (int i = 0; i < keys.size(); ++i){
		const QString& key = keys[i];
		QString jsonKey = getJSONKeyForProperty(key);
		QVariant value = property(key.toUtf8());
		result += key + ":";

		qDebug() << "value" << value;
		if (value.canConvert<CItemModelBase>()){
			qDebug() << "canConvert" << value;
			CItemModelBase* objectPtr = value.value<CItemModelBase*>();
			if (objectPtr == nullptr){
				return QString();
			}

			result += jsonKey + ':' + objectPtr->toGraphQL();
		}
		else if (value.typeId() == QMetaType::QString || value.typeId() == QMetaType::QByteArray){
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


QVariant CItemModelBase::CreateObject(const QString& /*key*/)
{
	return QVariant();
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
		if (name.startsWith("m_")){
			list.append(name);
		}
	}

	return list;
}


QString CItemModelBase::getJSONKeyForProperty(const QString& propertyName) const
{
	return propertyName;
}


CItemModelBase* CItemModelBase::CreateItemModel(const QString& /*jsonKey*/)
{
	return nullptr;
}


} // namespace imtbase


