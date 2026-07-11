// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

// ImtCore includes
#include <imtqml/CObjectViewModel.h>


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

	In addition to the raw \c model QVariant the controller owns a
	declarative \c CObjectViewModel exposed through the \c viewModel
	property. QML editors bind their input controls directly to the
	ViewModel properties; user edits are written back through
	\c submit() (form mode) or automatically after every edit when
	\c autoSubmit is enabled (live mode). \c revert() discards
	pending edits. When \c live is enabled the controller subscribes
	to the bridge for server/model pushed updates, which are applied
	to the ViewModel with central echo suppression — QML never needs
	blocker logic.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c DataModelController.
*/
class CDataModelController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString modelId READ GetModelId WRITE SetModelId NOTIFY modelIdChanged)
	Q_PROPERTY(QVariantMap parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)
	Q_PROPERTY(QVariant model READ GetModel NOTIFY modelChanged)
	Q_PROPERTY(bool isLoading READ IsLoading NOTIFY isLoadingChanged)
	Q_PROPERTY(imtqml::CObjectViewModel* viewModel READ GetViewModel CONSTANT)
	Q_PROPERTY(QString error READ GetError NOTIFY errorChanged)
	Q_PROPERTY(bool isDirty READ IsDirty NOTIFY isDirtyChanged)
	Q_PROPERTY(bool autoSubmit READ IsAutoSubmit WRITE SetAutoSubmit NOTIFY autoSubmitChanged)
	Q_PROPERTY(bool live READ IsLive WRITE SetLive NOTIFY liveChanged)

public:
	typedef QObject BaseClass;

	explicit CDataModelController(QObject* parent = nullptr);
	~CDataModelController() override;

	const QString& GetModelId() const;
	void SetModelId(const QString& modelId);

	const QVariantMap& GetParameters() const;
	void SetParameters(const QVariantMap& parameters);

	const QVariant& GetModel() const;
	bool IsLoading() const;

	CObjectViewModel* GetViewModel() const;
	const QString& GetError() const;
	bool IsDirty() const;

	bool IsAutoSubmit() const;
	void SetAutoSubmit(bool autoSubmit);

	bool IsLive() const;
	void SetLive(bool live);

public Q_SLOTS:
	/**
		\brief Request the model. Asynchronous: emits
		\c startGetModel(), then either \c modelReceived(model) or
		\c getModelFailed(message) on the GUI thread once done.

		Delegates to the \c IDataModelBridge resolved via
		\c CDataModelBridgeDemultiplexer::Instance(). If no bridge
		is available or no delegate claims \c modelId, emits
		\c getModelFailed with a descriptive message.
	*/
	virtual void getModel();

	/**
		\brief Push a new model. Asynchronous: emits
		\c startSetModel(model), then either \c modelSet() or
		\c setModelFailed(message) on the GUI thread once done.

		Delegates to the \c IDataModelBridge resolved via
		\c CDataModelBridgeDemultiplexer::Instance(). If no bridge
		is available or no delegate claims \c modelId, emits
		\c setModelFailed with a descriptive message.
	*/
	virtual void setModel(const QVariant& model);

	/**
		\brief Write all pending ViewModel edits back through the
		bridge (form mode). Delegates to \c setModel() with the
		current ViewModel values; on success the ViewModel is marked
		clean.
	*/
	void submit();

	/**
		\brief Discard all pending ViewModel edits and restore the
		values of the last source update.
	*/
	void revert();

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

	/**
		\brief Helper: set / clear the \c error property and emit
		\c errorChanged.
	*/
	void SetError(const QString& error);

Q_SIGNALS:
	void modelIdChanged(const QString& modelId);
	void parametersChanged(const QVariantMap& parameters);
	void modelChanged(const QVariant& model);
	void isLoadingChanged(bool isLoading);
	void errorChanged(const QString& error);
	void isDirtyChanged(bool isDirty);
	void autoSubmitChanged(bool autoSubmit);
	void liveChanged(bool live);

	void startGetModel();
	void modelReceived(const QVariant& model);
	void getModelFailed(const QString& message);

	void startSetModel(const QVariant& model);
	void modelSet();
	void setModelFailed(const QString& message);

private:
	void ApplySourceModel(const QVariant& model);
	void UpdateSubscription();
	void OnViewModelEdited();

private:
	QString m_modelId;
	QVariantMap m_parameters;
	QVariant m_model;
	bool m_isLoading = false;

	CObjectViewModel* m_viewModelPtr = nullptr;
	QString m_error;
	bool m_isAutoSubmit = false;
	bool m_isLive = false;
	int m_subscriptionId = 0;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CDataModelController*)
