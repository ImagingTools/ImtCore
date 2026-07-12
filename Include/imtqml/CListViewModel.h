// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QAbstractListModel>
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <QtCore/QVariantList>
#include <QtCore/QVector>


namespace imtqml
{


/**
	\brief Declarative role-based list adapter used by QML collection
	editors to bind a list-valued data-model field via a role model.

	\details
	\c CListViewModel is the list/collection counterpart of
	\c CObjectViewModel. Where the scalar ViewModel exposes every
	model field as a QML property, this adapter exposes a list-valued
	field as a \c QAbstractListModel: a \c ListView / \c Repeater binds
	directly to it and each element field appears as a named role, so
	collection editors bind their delegates instead of imperatively
	copying rows in \c updateGui() / \c updateModel() pairs.

	Two data flows are strictly separated, exactly like
	\c CObjectViewModel:

	- \b Source \b updates (bridge / transport → adapter): use
	  \c SetSourceValues(). Source updates reset the dirty state,
	  refresh the revert snapshot and do NOT emit \c changed — this is
	  the single, central place where the update-feedback loop is
	  broken. QML never needs blocker logic.
	- \b User \b edits (QML → adapter): the \c append(), \c insert(),
	  \c remove() and \c setProperty() slots (and \c setData() from a
	  bound view). They mark the adapter dirty and emit \c changed so
	  that the owning \c CObjectViewModel can write the change back to
	  the underlying data model (immediately in live mode, or on
	  \c submit() in form mode).

	Each element may be a \c QVariantMap (its keys become named roles)
	or a plain scalar (multi-select id lists). The whole element is
	always available through the \c modelData role, so a delegate can
	bind \c model.modelData for scalar lists and \c model.<field> for
	object lists.

	The type is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as an uncreatable \c ListViewModel.
	Instances are created and owned by \c CObjectViewModel and reached
	from QML through the corresponding ViewModel property.

	\sa CObjectViewModel, CDataModelController, IDataModelBridge
*/
class CListViewModel: public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
	Q_PROPERTY(bool isDirty READ IsDirty NOTIFY isDirtyChanged)

public:
	typedef QAbstractListModel BaseClass;

	/**
		\brief Role returning the whole element (map or scalar).
	*/
	enum Roles
	{
		ModelDataRole = Qt::UserRole + 1,
		DynamicRole
	};

	explicit CListViewModel(QObject* parent = nullptr);

	/**
		\brief Returns \c true if the adapter contains user edits
		that were not yet written back to the data model.
	*/
	bool IsDirty() const;

	/**
		\brief Replace the adapter content with values coming from the
		data source (bridge, transport, observer).

		\details
		This is the ONLY entry point for source-driven updates.
		It resets the rows, rebuilds the roles, takes a new revert
		snapshot, clears the dirty state and suppresses \c changed
		notifications, so no echo is sent back to the source.
	*/
	void SetSourceValues(const QVariantList& rows);

	/**
		\brief Returns the current rows as a \c QVariantList (element
		shape preserved: maps stay maps, scalars stay scalars).
	*/
	QVariantList GetValues() const;

	/**
		\brief Marks the current state as clean (e.g. after a
		successful submit) and refreshes the revert snapshot.
	*/
	void MarkClean();

	// reimplemented (QAbstractListModel)
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	virtual QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
	/**
		\brief Returns the element at \c row as a \c QVariantMap
		(scalars are wrapped under the \c modelData key). Empty map
		for an out-of-range row.
	*/
	QVariantMap get(int row) const;

	/**
		\brief User edit: set a single field of the element at \c row.
		Marks the adapter dirty and emits \c changed.
	*/
	void setProperty(int row, const QString& key, const QVariant& value);

	/**
		\brief User edit: append an element (map or scalar) at the end.
	*/
	void append(const QVariant& element);

	/**
		\brief User edit: insert an element (map or scalar) at \c row.
	*/
	void insert(int row, const QVariant& element);

	/**
		\brief User edit: remove the element at \c row.
	*/
	void remove(int row);

	/**
		\brief Discards all user edits and restores the rows of the
		last source update.
	*/
	void revert();

Q_SIGNALS:
	void countChanged();
	void isDirtyChanged(bool isDirty);

	/**
		\brief Emitted for user/QML edits only — never for source
		updates applied via \c SetSourceValues().
	*/
	void changed();

private:
	void RebuildRoles();
	bool RegisterKeys(const QVariantMap& map);
	void SetIsDirty(bool isDirty);
	void OnUserEdit();

private:
	QVector<QVariant> m_rows;
	QVariantList m_snapshot;
	QHash<int, QByteArray> m_roleNames;
	QHash<QString, int> m_roleIds;
	int m_nextRoleId = DynamicRole;
	bool m_isDirty = false;
	bool m_isSourceUpdate = false;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CListViewModel*)
