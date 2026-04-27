// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlComponentRegistry.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>


namespace imtqml
{


CQmlComponentRegistry* CQmlComponentRegistry::s_instance = nullptr;


CQmlComponentRegistry* CQmlComponentRegistry::Instance()
{
	if (!s_instance){
		s_instance = new CQmlComponentRegistry();
	}

	return s_instance;
}


CQmlComponentRegistry::CQmlComponentRegistry(QObject* parent)
	: QObject(parent)
{
}


CQmlComponentRegistry::~CQmlComponentRegistry()
{
	if (s_instance == this){
		s_instance = nullptr;
	}
}


int CQmlComponentRegistry::GetComponentCount() const
{
	return m_components.size();
}


bool CQmlComponentRegistry::registerComponent(CQmlAcfComponent* component)
{
	if (!component){
		qWarning() << "CQmlComponentRegistry::registerComponent: null component";
		return false;
	}

	const QString& id = component->GetComponentId();
	if (id.isEmpty()){
		qWarning() << "CQmlComponentRegistry::registerComponent: empty componentId";
		return false;
	}

	if (m_components.contains(id)){
		qWarning() << "CQmlComponentRegistry::registerComponent: component already registered:" << id;
		return false;
	}

	m_components.insert(id, component);
	Q_EMIT componentCountChanged();
	Q_EMIT componentRegistered(id);

	return true;
}


bool CQmlComponentRegistry::unregisterComponent(const QString& componentId)
{
	auto it = m_components.find(componentId);
	if (it == m_components.end()){
		return false;
	}

	CQmlAcfComponent* component = it.value();
	component->Deactivate();
	m_components.erase(it);
	Q_EMIT componentCountChanged();
	Q_EMIT componentUnregistered(componentId);

	return true;
}


CQmlAcfComponent* CQmlComponentRegistry::getComponent(const QString& componentId) const
{
	return m_components.value(componentId, nullptr);
}


CQmlAcfComponent* CQmlComponentRegistry::getByInterface(const QString& interfaceName) const
{
	for (auto* component : m_components){
		if (component->implementsInterface(interfaceName)){
			return component;
		}
	}

	return nullptr;
}


QVariantList CQmlComponentRegistry::getAllByInterface(const QString& interfaceName) const
{
	QVariantList result;
	for (auto* component : m_components){
		if (component->implementsInterface(interfaceName)){
			result.append(QVariant::fromValue(component));
		}
	}

	return result;
}


QStringList CQmlComponentRegistry::componentIds() const
{
	return m_components.keys();
}


int CQmlComponentRegistry::resolveAll()
{
	int unresolvedCount = 0;
	for (auto* component : m_components){
		unresolvedCount += ResolveReferences(component);
	}

	// Activate all components that have all required references resolved
	for (auto* component : m_components){
		if (component->IsActive()){
			continue;
		}

		bool allRequired = true;
		for (const auto* ref : component->ReferenceList()){
			if (ref->IsRequired() && !ref->isResolved()){
				allRequired = false;
				break;
			}
		}

		if (allRequired){
			component->Activate();
		}
	}

	Q_EMIT referencesResolved();

	return unresolvedCount;
}


int CQmlComponentRegistry::resolveComponent(const QString& componentId)
{
	auto* component = getComponent(componentId);
	if (!component){
		return -1;
	}

	int unresolvedCount = ResolveReferences(component);

	bool allRequired = true;
	for (const auto* ref : component->ReferenceList()){
		if (ref->IsRequired() && !ref->isResolved()){
			allRequired = false;
			break;
		}
	}

	if (allRequired && !component->IsActive()){
		component->Activate();
	}

	return unresolvedCount;
}


bool CQmlComponentRegistry::loadConfiguration(const QString& jsonString)
{
	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);
	if (parseError.error != QJsonParseError::NoError){
		qWarning() << "CQmlComponentRegistry::loadConfiguration: JSON parse error:"
				   << parseError.errorString();
		return false;
	}

	QJsonObject root = doc.object();
	QJsonArray elements = root.value(QStringLiteral("elements")).toArray();
	if (elements.isEmpty()){
		qWarning() << "CQmlComponentRegistry::loadConfiguration: no elements found";
		return false;
	}

	for (const QJsonValue& val : elements){
		QJsonObject elem = val.toObject();
		QString id = elem.value(QStringLiteral("id")).toString();
		if (id.isEmpty()){
			continue;
		}

		auto* component = getComponent(id);
		if (!component){
			qWarning() << "CQmlComponentRegistry::loadConfiguration: component not found:" << id;
			continue;
		}

		// Apply attribute overrides
		QJsonObject attrs = elem.value(QStringLiteral("attributes")).toObject();
		for (auto attrIt = attrs.begin(); attrIt != attrs.end(); ++attrIt){
			auto* attr = component->findAttribute(attrIt.key());
			if (attr){
				attr->SetValue(attrIt.value().toVariant());
			}
		}

		// Apply reference wiring
		QJsonObject refs = elem.value(QStringLiteral("references")).toObject();
		for (auto refIt = refs.begin(); refIt != refs.end(); ++refIt){
			auto* ref = component->findReference(refIt.key());
			if (ref){
				ref->SetTargetId(refIt.value().toString());
			}
		}
	}

	return true;
}


QVariantMap CQmlComponentRegistry::exportGraph() const
{
	QVariantList nodes;
	QVariantList edges;

	for (auto it = m_components.constBegin(); it != m_components.constEnd(); ++it){
		const CQmlAcfComponent* comp = it.value();

		QVariantMap node;
		node[QStringLiteral("id")] = comp->GetComponentId();
		node[QStringLiteral("packageId")] = comp->GetPackageId();
		node[QStringLiteral("interfaces")] = comp->GetInterfaces();
		node[QStringLiteral("isActive")] = comp->IsActive();
		node[QStringLiteral("description")] = comp->GetDescription();
		node[QStringLiteral("attributes")] = comp->attributeValues();
		nodes.append(node);

		// Build edges from references
		for (const auto* ref : comp->ReferenceList()){
			if (!ref->GetTargetId().isEmpty() || ref->isResolved()){
				QVariantMap edge;
				edge[QStringLiteral("from")] = comp->GetComponentId();
				edge[QStringLiteral("to")] = ref->GetTargetId();
				edge[QStringLiteral("refId")] = ref->GetRefId();
				edge[QStringLiteral("interfaceName")] = ref->GetInterfaceName();
				edge[QStringLiteral("type")] = QStringLiteral("reference");
				edge[QStringLiteral("isResolved")] = ref->isResolved();
				edges.append(edge);
			}
		}

		// Build edges from factories
		for (const auto* fact : comp->FactoryList()){
			QVariantMap edge;
			edge[QStringLiteral("from")] = comp->GetComponentId();
			edge[QStringLiteral("to")] = QString();
			edge[QStringLiteral("factoryId")] = fact->GetFactoryId();
			edge[QStringLiteral("interfaceName")] = fact->GetInterfaceName();
			edge[QStringLiteral("type")] = QStringLiteral("factory");
			edges.append(edge);
		}
	}

	QVariantMap result;
	result[QStringLiteral("nodes")] = nodes;
	result[QStringLiteral("edges")] = edges;

	return result;
}


int CQmlComponentRegistry::ResolveReferences(CQmlAcfComponent* component)
{
	int unresolvedCount = 0;

	for (auto* ref : component->ReferenceList()){
		if (ref->isResolved()){
			continue;
		}

		// Try to resolve by explicit targetId
		if (!ref->GetTargetId().isEmpty()){
			auto* target = getComponent(ref->GetTargetId());
			if (target){
				ref->SetTarget(target);
				continue;
			}
		}

		// Try to resolve by interface name
		if (!ref->GetInterfaceName().isEmpty()){
			auto* target = getByInterface(ref->GetInterfaceName());
			if (target && target != component){
				ref->SetTarget(target);
				ref->SetTargetId(target->GetComponentId());
				continue;
			}
		}

		++unresolvedCount;
	}

	return unresolvedCount;
}


} // namespace imtqml
