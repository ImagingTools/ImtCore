// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlAcfComponent.h>


namespace imtqml
{


CQmlAcfComponent::CQmlAcfComponent(QObject* parent)
	: QObject(parent)
{
}


CQmlAcfComponent::~CQmlAcfComponent()
{
	if (m_isActive){
		Deactivate();
	}
}


const QString& CQmlAcfComponent::GetComponentId() const
{
	return m_componentId;
}


void CQmlAcfComponent::SetComponentId(const QString& componentId)
{
	if (m_componentId != componentId){
		m_componentId = componentId;
		Q_EMIT componentIdChanged();
	}
}


const QString& CQmlAcfComponent::GetPackageId() const
{
	return m_packageId;
}


void CQmlAcfComponent::SetPackageId(const QString& packageId)
{
	if (m_packageId != packageId){
		m_packageId = packageId;
		Q_EMIT packageIdChanged();
	}
}


const QStringList& CQmlAcfComponent::GetInterfaces() const
{
	return m_interfaces;
}


void CQmlAcfComponent::SetInterfaces(const QStringList& interfaces)
{
	if (m_interfaces != interfaces){
		m_interfaces = interfaces;
		Q_EMIT interfacesChanged();
	}
}


bool CQmlAcfComponent::IsActive() const
{
	return m_isActive;
}


const QString& CQmlAcfComponent::GetDescription() const
{
	return m_description;
}


void CQmlAcfComponent::SetDescription(const QString& description)
{
	if (m_description != description){
		m_description = description;
		Q_EMIT descriptionChanged();
	}
}


QQmlListProperty<CQmlAcfAttribute> CQmlAcfComponent::GetAttributes()
{
	return QQmlListProperty<CQmlAcfAttribute>(this, nullptr,
		&CQmlAcfComponent::AppendAttribute,
		&CQmlAcfComponent::AttributeCount,
		&CQmlAcfComponent::AttributeAt,
		&CQmlAcfComponent::ClearAttributes);
}


QQmlListProperty<CQmlAcfReference> CQmlAcfComponent::GetReferences()
{
	return QQmlListProperty<CQmlAcfReference>(this, nullptr,
		&CQmlAcfComponent::AppendReference,
		&CQmlAcfComponent::ReferenceCount,
		&CQmlAcfComponent::ReferenceAt,
		&CQmlAcfComponent::ClearReferences);
}


QQmlListProperty<CQmlAcfFactory> CQmlAcfComponent::GetFactories()
{
	return QQmlListProperty<CQmlAcfFactory>(this, nullptr,
		&CQmlAcfComponent::AppendFactory,
		&CQmlAcfComponent::FactoryCount,
		&CQmlAcfComponent::FactoryAt,
		&CQmlAcfComponent::ClearFactories);
}


QVariantMap CQmlAcfComponent::attributeValues() const
{
	QVariantMap result;
	for (const auto* attr : m_attributes){
		result.insert(attr->GetAttrId(), attr->resolvedValue());
	}

	return result;
}


QVariantList CQmlAcfComponent::referenceInfo() const
{
	QVariantList result;
	for (const auto* ref : m_references){
		QVariantMap entry;
		entry[QStringLiteral("refId")] = ref->GetRefId();
		entry[QStringLiteral("interfaceName")] = ref->GetInterfaceName();
		entry[QStringLiteral("targetId")] = ref->GetTargetId();
		entry[QStringLiteral("isResolved")] = ref->isResolved();
		result.append(entry);
	}

	return result;
}


QVariantList CQmlAcfComponent::factoryInfo() const
{
	QVariantList result;
	for (const auto* fact : m_factories){
		QVariantMap entry;
		entry[QStringLiteral("factoryId")] = fact->GetFactoryId();
		entry[QStringLiteral("interfaceName")] = fact->GetInterfaceName();
		result.append(entry);
	}

	return result;
}


CQmlAcfAttribute* CQmlAcfComponent::findAttribute(const QString& attrId) const
{
	for (auto* attr : m_attributes){
		if (attr->GetAttrId() == attrId){
			return attr;
		}
	}

	return nullptr;
}


CQmlAcfReference* CQmlAcfComponent::findReference(const QString& refId) const
{
	for (auto* ref : m_references){
		if (ref->GetRefId() == refId){
			return ref;
		}
	}

	return nullptr;
}


CQmlAcfFactory* CQmlAcfComponent::findFactory(const QString& factoryId) const
{
	for (auto* fact : m_factories){
		if (fact->GetFactoryId() == factoryId){
			return fact;
		}
	}

	return nullptr;
}


bool CQmlAcfComponent::implementsInterface(const QString& interfaceName) const
{
	return m_interfaces.contains(interfaceName);
}


void CQmlAcfComponent::Activate()
{
	if (!m_isActive){
		m_isActive = true;
		Q_EMIT isActiveChanged();
		Q_EMIT componentCreated();
	}
}


void CQmlAcfComponent::Deactivate()
{
	if (m_isActive){
		Q_EMIT componentDestroyed();
		m_isActive = false;
		Q_EMIT isActiveChanged();
	}
}


const QList<CQmlAcfAttribute*>& CQmlAcfComponent::AttributeList() const
{
	return m_attributes;
}


const QList<CQmlAcfReference*>& CQmlAcfComponent::ReferenceList() const
{
	return m_references;
}


const QList<CQmlAcfFactory*>& CQmlAcfComponent::FactoryList() const
{
	return m_factories;
}


// --- QQmlListProperty helpers: Attributes ---

void CQmlAcfComponent::AppendAttribute(QQmlListProperty<CQmlAcfAttribute>* list, CQmlAcfAttribute* attr)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);
	if (component){
		attr->setParent(component);
		component->m_attributes.append(attr);
	}
}


qsizetype CQmlAcfComponent::AttributeCount(QQmlListProperty<CQmlAcfAttribute>* list)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);

	return component ? component->m_attributes.size() : 0;
}


CQmlAcfAttribute* CQmlAcfComponent::AttributeAt(QQmlListProperty<CQmlAcfAttribute>* list, qsizetype index)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);

	return (component && index >= 0 && index < component->m_attributes.size())
		? component->m_attributes.at(index)
		: nullptr;
}


void CQmlAcfComponent::ClearAttributes(QQmlListProperty<CQmlAcfAttribute>* list)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);
	if (component){
		qDeleteAll(component->m_attributes);
		component->m_attributes.clear();
	}
}


// --- QQmlListProperty helpers: References ---

void CQmlAcfComponent::AppendReference(QQmlListProperty<CQmlAcfReference>* list, CQmlAcfReference* ref)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);
	if (component){
		ref->setParent(component);
		component->m_references.append(ref);
	}
}


qsizetype CQmlAcfComponent::ReferenceCount(QQmlListProperty<CQmlAcfReference>* list)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);

	return component ? component->m_references.size() : 0;
}


CQmlAcfReference* CQmlAcfComponent::ReferenceAt(QQmlListProperty<CQmlAcfReference>* list, qsizetype index)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);

	return (component && index >= 0 && index < component->m_references.size())
		? component->m_references.at(index)
		: nullptr;
}


void CQmlAcfComponent::ClearReferences(QQmlListProperty<CQmlAcfReference>* list)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);
	if (component){
		qDeleteAll(component->m_references);
		component->m_references.clear();
	}
}


// --- QQmlListProperty helpers: Factories ---

void CQmlAcfComponent::AppendFactory(QQmlListProperty<CQmlAcfFactory>* list, CQmlAcfFactory* fact)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);
	if (component){
		fact->setParent(component);
		component->m_factories.append(fact);
	}
}


qsizetype CQmlAcfComponent::FactoryCount(QQmlListProperty<CQmlAcfFactory>* list)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);

	return component ? component->m_factories.size() : 0;
}


CQmlAcfFactory* CQmlAcfComponent::FactoryAt(QQmlListProperty<CQmlAcfFactory>* list, qsizetype index)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);

	return (component && index >= 0 && index < component->m_factories.size())
		? component->m_factories.at(index)
		: nullptr;
}


void CQmlAcfComponent::ClearFactories(QQmlListProperty<CQmlAcfFactory>* list)
{
	auto* component = qobject_cast<CQmlAcfComponent*>(list->object);
	if (component){
		qDeleteAll(component->m_factories);
		component->m_factories.clear();
	}
}


} // namespace imtqml
