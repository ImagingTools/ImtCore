// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>


namespace imtqml
{


/**
	\brief Describes a dependency (reference) of an ACF QML component.
	\details This is the QML analog of the ACF I_REF mechanism.
	It declares that the owning component needs a reference to another
	component that implements the specified interface.

	Usage in QML:
	\code
	AcfComponent {
		AcfReference { refId: "AuthProvider"; interfaceName: "IAuthProvider"; required: true }
		AcfReference { refId: "Logger";       interfaceName: "ILogger";       required: false }
	}
	\endcode

	At runtime the CQmlComponentRegistry resolves each reference:
	it looks up the target component by \c targetId (direct wiring from
	configuration) or by \c interfaceName (interface-based lookup) and
	stores the resulting QObject pointer in the \c target property.
*/
class CQmlAcfReference: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString refId READ GetRefId WRITE SetRefId NOTIFY refIdChanged)
	Q_PROPERTY(QString interfaceName READ GetInterfaceName WRITE SetInterfaceName NOTIFY interfaceNameChanged)
	Q_PROPERTY(bool required READ IsRequired WRITE SetRequired NOTIFY requiredChanged)
	Q_PROPERTY(QString targetId READ GetTargetId WRITE SetTargetId NOTIFY targetIdChanged)
	Q_PROPERTY(QObject* target READ GetTarget WRITE SetTarget NOTIFY targetChanged)
	Q_PROPERTY(QString description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)

public:
	explicit CQmlAcfReference(QObject* parent = nullptr);

	const QString& GetRefId() const;
	void SetRefId(const QString& refId);

	const QString& GetInterfaceName() const;
	void SetInterfaceName(const QString& interfaceName);

	bool IsRequired() const;
	void SetRequired(bool required);

	const QString& GetTargetId() const;
	void SetTargetId(const QString& targetId);

	QObject* GetTarget() const;
	void SetTarget(QObject* target);

	const QString& GetDescription() const;
	void SetDescription(const QString& description);

	/**
		\brief Returns \c true if the reference has been resolved (target is non-null).
	*/
	Q_INVOKABLE bool isResolved() const;

Q_SIGNALS:
	void refIdChanged();
	void interfaceNameChanged();
	void requiredChanged();
	void targetIdChanged();
	void targetChanged();
	void descriptionChanged();

private:
	QString m_refId;
	QString m_interfaceName;
	bool m_required = false;
	QString m_targetId;
	QObject* m_target = nullptr;
	QString m_description;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CQmlAcfReference*)
