// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace imtqml
{


/**
	\brief Describes a single attribute (property) of an ACF QML component.
	\details This is the QML analog of the ACF I_ATTR / I_ASSIGN mechanism.
	It stores compile-time metadata about a property: identifier, data type,
	whether the property is required, and an optional default value.

	Usage in QML:
	\code
	AcfComponent {
		AcfAttribute { attrId: "ServerUrl"; type: "string"; required: true; defaultValue: "http://localhost" }
		AcfAttribute { attrId: "PageSize";  type: "int";    required: false; defaultValue: 20 }
	}
	\endcode
*/
class CQmlAcfAttribute: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString attrId READ GetAttrId WRITE SetAttrId NOTIFY attrIdChanged)
	Q_PROPERTY(QString type READ GetType WRITE SetType NOTIFY typeChanged)
	Q_PROPERTY(bool required READ IsRequired WRITE SetRequired NOTIFY requiredChanged)
	Q_PROPERTY(QVariant defaultValue READ GetDefaultValue WRITE SetDefaultValue NOTIFY defaultValueChanged)
	Q_PROPERTY(QVariant value READ GetValue WRITE SetValue NOTIFY valueChanged)
	Q_PROPERTY(QString description READ GetDescription WRITE SetDescription NOTIFY descriptionChanged)

public:
	explicit CQmlAcfAttribute(QObject* parent = nullptr);

	const QString& GetAttrId() const;
	void SetAttrId(const QString& attrId);

	const QString& GetType() const;
	void SetType(const QString& type);

	bool IsRequired() const;
	void SetRequired(bool required);

	const QVariant& GetDefaultValue() const;
	void SetDefaultValue(const QVariant& defaultValue);

	const QVariant& GetValue() const;
	void SetValue(const QVariant& value);

	const QString& GetDescription() const;
	void SetDescription(const QString& description);

	/**
		\brief Returns the effective value: the explicit value if set, otherwise the default.
	*/
	Q_INVOKABLE QVariant resolvedValue() const;

Q_SIGNALS:
	void attrIdChanged();
	void typeChanged();
	void requiredChanged();
	void defaultValueChanged();
	void valueChanged();
	void descriptionChanged();

private:
	QString m_attrId;
	QString m_type;
	bool m_required = false;
	QVariant m_defaultValue;
	QVariant m_value;
	QString m_description;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CQmlAcfAttribute*)
