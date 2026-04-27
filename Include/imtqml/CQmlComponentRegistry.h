// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtqml/CQmlAcfComponent.h>


namespace imtqml
{


/**
	\brief Central registry and dependency-injection container for QML ACF components.

	\details CQmlComponentRegistry is the QML analog of the ACF C++ component
	registry.  It is a singleton exposed to QML as
	\c com.imtcore.imtqml 1.0 \b AcfRegistry.

	Responsibilities:
	- Register QML component instances (by their \c componentId).
	- Resolve inter-component references (dependency injection).
	- Load component configuration from JSON.
	- Provide introspection API to enumerate all registered components,
	  their attributes, references, and connections.
	- Export the component graph as a JSON structure for visualization.

	Usage from QML:
	\code
	import com.imtcore.imtqml 1.0

	Component.onCompleted: {
		// Register a component
		AcfRegistry.registerComponent(myAcfComponent);

		// Resolve all outstanding references
		AcfRegistry.resolveAll();

		// Look up by interface
		var provider = AcfRegistry.getByInterface("IDataProvider");

		// Export graph for visualization
		var graph = AcfRegistry.exportGraph();
	}
	\endcode
*/
class CQmlComponentRegistry: public QObject
{
	Q_OBJECT
	Q_PROPERTY(int componentCount READ GetComponentCount NOTIFY componentCountChanged)

public:
	static CQmlComponentRegistry* Instance();

	explicit CQmlComponentRegistry(QObject* parent = nullptr);
	~CQmlComponentRegistry() override;

	int GetComponentCount() const;

	// --- Registration ---

	/**
		\brief Register a QML ACF component in the registry.
		\details The component is indexed by its \c componentId.  If a component
		with the same ID is already registered, the call is ignored and a
		warning is printed.
	*/
	Q_INVOKABLE bool registerComponent(imtqml::CQmlAcfComponent* component);

	/**
		\brief Remove a previously registered component.
	*/
	Q_INVOKABLE bool unregisterComponent(const QString& componentId);

	// --- Lookup ---

	/**
		\brief Look up a component by its \c componentId.
		Returns \c null if not found.
	*/
	Q_INVOKABLE imtqml::CQmlAcfComponent* getComponent(const QString& componentId) const;

	/**
		\brief Look up the first component that declares the given interface.
		Returns \c null if not found.
	*/
	Q_INVOKABLE imtqml::CQmlAcfComponent* getByInterface(const QString& interfaceName) const;

	/**
		\brief Return all components that declare the given interface.
	*/
	Q_INVOKABLE QVariantList getAllByInterface(const QString& interfaceName) const;

	/**
		\brief Return a list of all registered component IDs.
	*/
	Q_INVOKABLE QStringList componentIds() const;

	// --- Dependency injection ---

	/**
		\brief Resolve all outstanding references across all registered components.
		\details Iterates over every registered component's references.  For each
		reference that has a \c targetId, the registry looks up the component
		with that ID and sets \c target.  For references without a \c targetId
		but with an \c interfaceName, the first matching component is used.

		Returns the number of references that could \e not be resolved.
	*/
	Q_INVOKABLE int resolveAll();

	/**
		\brief Resolve references for a single component.
		Returns the number of references that could \e not be resolved.
	*/
	Q_INVOKABLE int resolveComponent(const QString& componentId);

	// --- Configuration ---

	/**
		\brief Load component configuration from a JSON string.
		\details Expects an object with an \c "elements" array.  Each element
		maps attribute values and reference wiring to an already-registered
		component.

		Example:
		\code{json}
		{
			"elements": [
				{
					"id": "MainDataProvider",
					"attributes": { "ServerUrl": "https://api.example.com" },
					"references": { "AuthProvider": "MainAuthProvider" }
				}
			]
		}
		\endcode
	*/
	Q_INVOKABLE bool loadConfiguration(const QString& jsonString);

	// --- Introspection / visualization ---

	/**
		\brief Export the full component graph as a QVariantMap.
		\details The result contains:
		- \c "nodes": array of component descriptors
		- \c "edges": array of reference connections
		This structure can be directly consumed by ComponentGraphView.qml.
	*/
	Q_INVOKABLE QVariantMap exportGraph() const;

Q_SIGNALS:
	void componentCountChanged();
	void componentRegistered(const QString& componentId);
	void componentUnregistered(const QString& componentId);
	void referencesResolved();

private:
	int ResolveReferences(CQmlAcfComponent* component);

	QHash<QString, CQmlAcfComponent*> m_components;

	static CQmlComponentRegistry* s_instance;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CQmlComponentRegistry*)
