// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace imtqml
{


/**
	\brief Generic asynchronous controller / provider for an arbitrary
	model.

	\details
	The class is intended to be the common base for every model
	provider used from QML. It exposes two operations — \c getModel()
	and \c setModel() — which are by contract \b asynchronous: each
	operation first emits a \c startGet startSet* signal, then (after
	the underlying transport finishes, possibly on a worker thread)
	emits the matching success or failure signal on the GUI thread.

	The base implementation is a no-op stub that mirrors the
	convention used by the controllers' start/success signal: it
	emits the \c start* signal, logs a \c qWarning that the method
	should be reimplemented in a subclass, and emits the success
	signal with the currently cached \c model.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c DataModelController.
*/
class CDataModelController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString modelId READ GetModelId WRITE SetModelId NOTIFY modelIdChanged)
	Q_PROPERTY(QVariant model READ GetModel NOTIFY modelChanged)
	Q_PROPERTY(bool isLoading READ IsLoading NOTIFY isLoadingChanged)

public:
	typedef QObject BaseClass;

	explicit CDataModelController(QObject* parent = nullptr);
	~CDataModelController() override;

	const QString& GetModelId() const;
	void SetModelId(const QString& modelId);

	const QVariant& GetModel() const;
	bool IsLoading() const;

public Q_SLOTS:
	/**
		\brief Request the model. Asynchronous: emits
		\c startGetModel(), then either \c modelReceived(model) or
		\c getModelFailed(message) on the GUI thread once done.

		The base implementation is a no-op stub; subclasses MUST
		override and perform the actual work.
	*/
	virtual void getModel();

	/**
		\brief Push a new model. Asynchronous: emits
		\c startSetModel(model), then either \c modelSet() or
		\c setModelFailed(message) on the GUI thread once done.

		The base implementation is a no-op stub; subclasses MUST
		override and perform the actual work.
	*/
	virtual void setModel(const QVariant& model);

protected:
	/**
		\brief Helper for subclasses: update the cached \c model
		property and emit \c modelChanged.
	*/
	void UpdateCachedModel(const QVariant& model);

	/**
		\brief Helper for subclasses: update the \c isLoading flag
		and emit \c isLoadingChanged.
	*/
	void SetIsLoading(bool isLoading);

Q_SIGNALS:
	void modelIdChanged(const QString& modelId);
	void modelChanged(const QVariant& model);
	void isLoadingChanged(bool isLoading);

	void startGetModel();
	void modelReceived(const QVariant& model);
	void getModelFailed(const QString& message);

	void startSetModel(const QVariant& model);
	void modelSet();
	void setModelFailed(const QString& message);

private:
	QString m_modelId;
	QVariant m_model;
	bool m_isLoading = false;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CDataModelController*)
