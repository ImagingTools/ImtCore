// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QVariantMap>
#include <QtQml/QQmlPropertyMap>

// ImtCore includes
#include <imtqml/CListViewModel.h>


namespace imtqml
{


/**
	\brief Declarative QObject-based ViewModel used by QML editors to
	bind data-model fields via property bindings.

	\details
	\c CObjectViewModel is the QML-facing representation of a single
	data-model object. It is based on \c QQmlPropertyMap, so every
	model field appears as a regular QML property with automatic
	change notification — QML editors bind their input controls
	directly to these properties instead of implementing imperative
	\c updateGui() / \c updateModel() function pairs.

	Two data flows are strictly separated:

	- \b Source \b updates (bridge / transport → ViewModel): use
	  \c SetSourceValues(). Source updates reset the dirty state,
	  refresh the revert snapshot and do NOT emit \c valueEdited —
	  this is the single, central place where the update-feedback
	  loop is broken. QML never needs blocker logic
	  (\c blockingUpdateModel, \c UpdateBlocker etc.).
	- \b User \b edits (QML → ViewModel): plain property writes from
	  QML. They mark the ViewModel dirty and emit \c valueEdited so
	  that a controller or bridge can write the change back to the
	  underlying data model (immediately in live mode, or on
	  \c submit() in form mode).

	List / collection fields (a field whose source value is a
	\c QVariantList) are not exposed as plain scalar properties but as
	a dedicated \c CListViewModel role adapter, created lazily on the
	first source update and reused across updates so QML bindings stay
	stable. The property value for such a key is the adapter object, so
	a \c ListView / \c Repeater binds to \c model.<field> while scalar
	controls keep binding to \c model.<scalarField>. Edits made through
	the adapter mark the ViewModel dirty and are written back exactly
	like scalar edits (through \c GetValues() / \c GetChangedValues()).

	The type is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c ObjectViewModel. Instances
	are typically created and owned by \c CDataModelController and
	consumed in QML through its \c viewModel property.

	\sa CDataModelController, CListViewModel, IDataModelBridge
*/
class CObjectViewModel: public QQmlPropertyMap
{
	Q_OBJECT
	Q_PROPERTY(bool isDirty READ IsDirty NOTIFY isDirtyChanged)
	Q_PROPERTY(bool hasErrors READ HasErrors NOTIFY fieldErrorsChanged)

public:
	typedef QQmlPropertyMap BaseClass;

	explicit CObjectViewModel(QObject* parent = nullptr);

	/**
		\brief Returns \c true if the ViewModel contains user edits
		that were not yet written back to the data model.
	*/
	bool IsDirty() const;

	/**
		\brief Returns \c true if at least one field currently carries a
		non-empty validation error message.

		\sa SetFieldErrors(), fieldError()
	*/
	bool HasErrors() const;

	/**
		\brief Replace the per-field validation errors of the ViewModel.

		\details
		Validation results are produced outside the ViewModel (e.g. by a
		controller or bridge after a submit / server round-trip) and are
		pushed in as a map of field key to error message. An empty or
		absent message means the field is valid. QML editors read the
		message for a field through \c fieldError() to drive
		\c errorText / \c showErrorWhenInvalid from the ViewModel instead
		of a purely client-side validator.

		Passing an empty map clears all field errors. Field errors are
		also cleared automatically on the next \c SetSourceValues(), since
		a fresh source snapshot invalidates previous validation results.
	*/
	void SetFieldErrors(const QVariantMap& errors);

	/**
		\brief Replace / merge the ViewModel content with values
		coming from the data source (bridge, transport, observer).

		\details
		This is the ONLY entry point for source-driven updates.
		It updates the properties, takes a new revert snapshot,
		clears the dirty state and suppresses \c valueEdited
		notifications, so no echo is sent back to the source.
	*/
	void SetSourceValues(const QVariantMap& values);

	/**
		\brief Returns the current values of all properties.
	*/
	QVariantMap GetValues() const;

	/**
		\brief Returns only the values changed by the user since the
		last \c SetSourceValues() / \c MarkClean() call.
	*/
	QVariantMap GetChangedValues() const;

	/**
		\brief Returns the list adapter bound to \c key, or \c nullptr
		if the key does not hold a collection field.
	*/
	CListViewModel* GetListAdapter(const QString& key) const;

	/**
		\brief Marks the current state as clean (e.g. after a
		successful submit) and refreshes the revert snapshot.
	*/
	void MarkClean();

public Q_SLOTS:
	/**
		\brief Discards all user edits and restores the values of the
		last source update.
	*/
	void revert();

	/**
		\brief Returns the validation error message for \a key, or an
		empty string if the field is valid.

		\details
		Exposed as a slot so QML editors can bind
		\c errorText: model ? model.fieldError("username") : "".
	*/
	QString fieldError(const QString& key) const;

Q_SIGNALS:
	void isDirtyChanged(bool isDirty);

	/**
		\brief Emitted whenever the per-field validation errors change
		(via \c SetFieldErrors() or when they are cleared by a source
		update).
	*/
	void fieldErrorsChanged();

	/**
		\brief Emitted for user/QML edits only — never for source
		updates applied via \c SetSourceValues().
	*/
	void valueEdited(const QString& key, const QVariant& value);

protected:
	// reimplemented (QQmlPropertyMap)
	virtual QVariant updateValue(const QString& key, const QVariant& input) override;

private:
	void SetIsDirty(bool isDirty);
	CListViewModel* GetOrCreateListAdapter(const QString& key);
	void OnListAdapterChanged(const QString& key);

private:
	QVariantMap m_snapshot;
	QVariantMap m_changedValues;
	QVariantMap m_fieldErrors;
	QHash<QString, CListViewModel*> m_listAdapters;
	bool m_isDirty = false;
	bool m_isSourceUpdate = false;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CObjectViewModel*)
