// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>


namespace imtqml
{


/**
	\brief Describes a factory slot of an ACF QML component.
	\details This is the QML analog of the ACF I_FACT mechanism.
	It declares that the owning component can create instances of
	components implementing a given interface on demand.

	Usage in QML:
	\code
	AcfComponent {
		AcfFactory { factoryId: "ItemFactory"; interfaceName: "IListItem" }
	}
	\endcode

	At runtime the CQmlComponentRegistry resolves the factory by looking
	up the component URL that implements the requested interface and stores
	it in \c sourceUrl.  Calling \c createInstance() from QML creates a
	new QObject via \c Qt.createComponent / \c Component.createObject.
*/
class CQmlAcfFactory: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString factoryId READ GetFactoryId WRITE SetFactoryId NOTIFY factoryIdChanged)
	Q_PROPERTY(QString interfaceName READ GetInterfaceName WRITE SetInterfaceName NOTIFY interfaceNameChanged)
	Q_PROPERTY(QUrl sourceUrl READ GetSourceUrl WRITE SetSourceUrl NOTIFY sourceUrlChanged)
	Q_PROPERTY(QString description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)

public:
	explicit CQmlAcfFactory(QObject* parent = nullptr);

	const QString& GetFactoryId() const;
	void SetFactoryId(const QString& factoryId);

	const QString& GetInterfaceName() const;
	void SetInterfaceName(const QString& interfaceName);

	const QUrl& GetSourceUrl() const;
	void SetSourceUrl(const QUrl& sourceUrl);

	const QString& GetDescription() const;
	void SetDescription(const QString& description);

Q_SIGNALS:
	void factoryIdChanged();
	void interfaceNameChanged();
	void sourceUrlChanged();
	void descriptionChanged();

private:
	QString m_factoryId;
	QString m_interfaceName;
	QUrl m_sourceUrl;
	QString m_description;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CQmlAcfFactory*)
