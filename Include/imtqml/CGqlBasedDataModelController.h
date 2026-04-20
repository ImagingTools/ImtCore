// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>

// ImtCore includes
#include <imtqml/CDataModelController.h>


namespace imtqml
{


class CGqlClientBridge;


/**
	\brief Asynchronous \c CDataModelController backed by an SDL model
	served through the \c imtclientgql GraphQL pipeline.

	\details
	The base class provides the asynchronous machinery (worker thread
	via \c QtConcurrent::run, signal marshalling back to the GUI
	thread, \c isLoading flag bookkeeping). Concrete subclasses bind to
	a specific SDL request/payload pair by overriding the protected
	hooks \c FetchModel() / \c StoreModel(); the hooks are invoked on
	the worker thread and typically use
	\c CGqlClientBridge::SendSdlRequest<>.

	From QML the user picks the model and the SDL parameters by
	setting \c modelId / \c parameters and calling
	\c getModel() / \c setModel(model).

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as
	\c GqlBasedDataModelController.

	\note Like every other GQL-based controller in this package, the
	default API client is the \c GqlClientBridge singleton; an
	alternative bridge can be supplied via \c apiClient.
*/
class CGqlBasedDataModelController: public CDataModelController
{
	Q_OBJECT
	Q_PROPERTY(QObject* apiClient READ GetApiClient WRITE SetApiClient NOTIFY apiClientChanged)
	Q_PROPERTY(QVariantMap parameters READ GetParameters WRITE SetParameters NOTIFY parametersChanged)

public:
	typedef CDataModelController BaseClass;

	explicit CGqlBasedDataModelController(QObject* parent = nullptr);
	~CGqlBasedDataModelController() override;

	QObject* GetApiClient() const;
	void SetApiClient(QObject* apiClient);

	const QVariantMap& GetParameters() const;
	void SetParameters(const QVariantMap& parameters);

public Q_SLOTS:
	// reimplemented (CDataModelController)
	void getModel() override;
	void setModel(const QVariant& model) override;

Q_SIGNALS:
	void apiClientChanged(QObject* apiClient);
	void parametersChanged(const QVariantMap& parameters);

protected:
	/**
		\brief Worker-thread hook: fetch the model identified by
		\c modelId with the supplied \c parameters using the
		injected \c bridge. Subclasses implementing a specific SDL
		request override this method, perform
		\c bridge->SendSdlRequest<...>(...) (or any other blocking
		API), and return the deserialised model.

		\param[in]  bridge        Resolved GQL client bridge (never
		                          \c nullptr when invoked).
		\param[in]  modelId       The \c modelId property at the time
		                          the request was issued.
		\param[in]  parameters    The \c parameters property at the
		                          time the request was issued.
		\param[out] errorMessage  Empty on success, otherwise carries
		                          the textual diagnostic.
		\returns The fetched model as a \c QVariant. Ignored when
		         \c errorMessage is non-empty.

		The base implementation populates \c errorMessage with
		"Not implemented" and returns an invalid \c QVariant.
	*/
	virtual QVariant FetchModel(
			CGqlClientBridge* bridge,
			const QString& modelId,
			const QVariantMap& parameters,
			QString& errorMessage) const;

	/**
		\brief Worker-thread hook: push \c model identified by
		\c modelId with \c parameters to the server through
		\c bridge. Same calling convention as \c FetchModel.
	*/
	virtual bool StoreModel(
			CGqlClientBridge* bridge,
			const QString& modelId,
			const QVariantMap& parameters,
			const QVariant& model,
			QString& errorMessage) const;

	/**
		\brief Resolve the bridge to use for the next request:
		\c apiClient (if it casts to \c CGqlClientBridge) or the
		singleton instance.
	*/
	CGqlClientBridge* ResolveBridge() const;

private:
	QObject* m_apiClient = nullptr;
	QVariantMap m_parameters;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlBasedDataModelController*)
