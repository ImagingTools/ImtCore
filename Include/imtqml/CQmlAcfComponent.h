// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
#include <QtQml/QQmlListProperty>

// ImtCore includes
#include <imtqml/CQmlAcfAttribute.h>
#include <imtqml/CQmlAcfReference.h>
#include <imtqml/CQmlAcfFactory.h>


namespace imtqml
{


/**
	\brief Base QML component type analogous to ACF's CComponentBase.
	\details Provides an ACF-like component model for QML. Each instance
	carries metadata (componentId, packageId, interfaces) and manages
	child descriptors: AcfAttribute, AcfReference, and AcfFactory.

	The component follows the same lifecycle as ACF C++ components:
	- componentCreated() signal is emitted after all attributes/references
	  have been resolved by the CQmlComponentRegistry.
	- componentDestroyed() signal is emitted when the component is being
	  removed from the registry.

	Usage in QML:
	\code
	import com.imtcore.imtqml 1.0

	AcfComponent {
		componentId: "MyDataProvider"
		packageId: "imtguigql"
		interfaces: ["IDataProvider", "IFilterable"]

		AcfAttribute { attrId: "ServerUrl"; type: "string"; required: true; defaultValue: "http://localhost" }
		AcfReference { refId: "AuthProvider"; interfaceName: "IAuthProvider"; required: true }
		AcfFactory   { factoryId: "ItemFactory"; interfaceName: "IListItem" }
	}
	\endcode
*/
class CQmlAcfComponent: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString componentId READ GetComponentId WRITE SetComponentId NOTIFY componentIdChanged)
	Q_PROPERTY(QString packageId READ GetPackageId WRITE SetPackageId NOTIFY packageIdChanged)
	Q_PROPERTY(QStringList interfaces READ GetInterfaces WRITE SetInterfaces NOTIFY interfacesChanged)
	Q_PROPERTY(bool isActive READ IsActive NOTIFY isActiveChanged)
	Q_PROPERTY(QString description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QQmlListProperty<imtqml::CQmlAcfAttribute> attributes READ GetAttributes)
	Q_PROPERTY(QQmlListProperty<imtqml::CQmlAcfReference> references READ GetReferences)
	Q_PROPERTY(QQmlListProperty<imtqml::CQmlAcfFactory> factories READ GetFactories)
	Q_CLASSINFO("DefaultProperty", "attributes")

public:
	explicit CQmlAcfComponent(QObject* parent = nullptr);
	~CQmlAcfComponent() override;

	const QString& GetComponentId() const;
	void SetComponentId(const QString& componentId);

	const QString& GetPackageId() const;
	void SetPackageId(const QString& packageId);

	const QStringList& GetInterfaces() const;
	void SetInterfaces(const QStringList& interfaces);

	bool IsActive() const;

	const QString& GetDescription() const;
	void SetDescription(const QString& description);

	QQmlListProperty<CQmlAcfAttribute> GetAttributes();
	QQmlListProperty<CQmlAcfReference> GetReferences();
	QQmlListProperty<CQmlAcfFactory> GetFactories();

	// --- Introspection API ---

	/**
		\brief Returns a QVariantMap snapshot of all attributes
		(attrId -> resolvedValue).
	*/
	Q_INVOKABLE QVariantMap attributeValues() const;

	/**
		\brief Returns a QVariantList of reference descriptors.
		Each entry is a QVariantMap { refId, interfaceName, targetId, isResolved }.
	*/
	Q_INVOKABLE QVariantList referenceInfo() const;

	/**
		\brief Returns a QVariantList of factory descriptors.
		Each entry is a QVariantMap { factoryId, interfaceName }.
	*/
	Q_INVOKABLE QVariantList factoryInfo() const;

	/**
		\brief Look up an attribute by its attrId.
	*/
	Q_INVOKABLE imtqml::CQmlAcfAttribute* findAttribute(const QString& attrId) const;

	/**
		\brief Look up a reference by its refId.
	*/
	Q_INVOKABLE imtqml::CQmlAcfReference* findReference(const QString& refId) const;

	/**
		\brief Look up a factory by its factoryId.
	*/
	Q_INVOKABLE imtqml::CQmlAcfFactory* findFactory(const QString& factoryId) const;

	/**
		\brief Returns true if this component declares the given interface name.
	*/
	Q_INVOKABLE bool implementsInterface(const QString& interfaceName) const;

	// --- Lifecycle ---

	/**
		\brief Called by the registry after all references have been resolved.
	*/
	void Activate();

	/**
		\brief Called by the registry when the component is being removed.
	*/
	void Deactivate();

	// --- Internal list access ---
	const QList<CQmlAcfAttribute*>& AttributeList() const;
	const QList<CQmlAcfReference*>& ReferenceList() const;
	const QList<CQmlAcfFactory*>& FactoryList() const;

Q_SIGNALS:
	void componentIdChanged();
	void packageIdChanged();
	void interfacesChanged();
	void isActiveChanged();
	void descriptionChanged();

	/**
		\brief Emitted after all references are resolved and the component is ready.
	*/
	void componentCreated();

	/**
		\brief Emitted when the component is being removed from the registry.
	*/
	void componentDestroyed();

private:
	// QQmlListProperty helpers for attributes
	static void AppendAttribute(QQmlListProperty<CQmlAcfAttribute>* list, CQmlAcfAttribute* attr);
	static qsizetype AttributeCount(QQmlListProperty<CQmlAcfAttribute>* list);
	static CQmlAcfAttribute* AttributeAt(QQmlListProperty<CQmlAcfAttribute>* list, qsizetype index);
	static void ClearAttributes(QQmlListProperty<CQmlAcfAttribute>* list);

	// QQmlListProperty helpers for references
	static void AppendReference(QQmlListProperty<CQmlAcfReference>* list, CQmlAcfReference* ref);
	static qsizetype ReferenceCount(QQmlListProperty<CQmlAcfReference>* list);
	static CQmlAcfReference* ReferenceAt(QQmlListProperty<CQmlAcfReference>* list, qsizetype index);
	static void ClearReferences(QQmlListProperty<CQmlAcfReference>* list);

	// QQmlListProperty helpers for factories
	static void AppendFactory(QQmlListProperty<CQmlAcfFactory>* list, CQmlAcfFactory* fact);
	static qsizetype FactoryCount(QQmlListProperty<CQmlAcfFactory>* list);
	static CQmlAcfFactory* FactoryAt(QQmlListProperty<CQmlAcfFactory>* list, qsizetype index);
	static void ClearFactories(QQmlListProperty<CQmlAcfFactory>* list);

	QString m_componentId;
	QString m_packageId;
	QStringList m_interfaces;
	bool m_isActive = false;
	QString m_description;

	QList<CQmlAcfAttribute*> m_attributes;
	QList<CQmlAcfReference*> m_references;
	QList<CQmlAcfFactory*> m_factories;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CQmlAcfComponent*)
