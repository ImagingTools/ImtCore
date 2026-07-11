// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QVariantMap>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtclientgql/IGqlSubscriptionManager.h>
#include <imtgql/CGqlRequest.h>
#include <imtqml/CDataModelBridgeBase.h>


namespace imtqml
{


/**
	\brief GraphQL (web) implementation of \c IDataModelBridge —
	the remote counterpart of \c TLocalDataModelBridgeComp.

	\details
	The bridge serves a single \c modelId (inherited \c ModelId
	attribute) from a GraphQL endpoint. The three transport
	operations map 1:1 onto the GraphQL operation types:

	- \c GetModel sends a \b query (\c QueryCommandId attribute),
	  passing the controller \c parameters as the \c input argument
	  object and selecting the payload fields listed in the
	  \c Fields attribute;
	- \c SetModel sends a \b mutation (\c MutationCommandId
	  attribute) whose \c input object is the merge of the
	  controller \c parameters and the ViewModel values, selecting
	  the fields listed in \c MutationFields (default: \c success);
	- \c SubscribeModel registers a GraphQL \b subscription
	  (\c SubscriptionCommandId attribute) over the WebSocket
	  channel of the referenced \c IGqlSubscriptionManager; every
	  pushed payload is decoded and forwarded to the subscriber.

	Payload decoding is generic: the \c data object of the GraphQL
	response is unwrapped, the object below the command ID is
	converted into the \c QVariantMap consumed by
	\c CObjectViewModel. Field selections support nested objects via
	dotted paths (e.g. \c address.city).

	All callbacks are delivered on the GUI thread
	(\c IDataModelBridge contract); the network / WebSocket threads
	never touch the ViewModel directly.

	Because the QML editor only ever talks to
	\c CDataModelController, plugging this bridge instead of a
	\c TLocalDataModelBridgeComp delegate into
	\c CDataModelBridgeDemultiplexer switches a screen from the
	in-process desktop transport to GraphQL without any QML change.

	\sa IDataModelBridge, CDataModelBridgeBase,
		CDataModelBridgeDemultiplexer, TLocalDataModelBridgeComp
*/
class CGqlDataModelBridgeComp:
			public CDataModelBridgeBase,
			virtual public imtclientgql::IGqlSubscriptionClient
{
public:
	typedef CDataModelBridgeBase BaseClass;

	I_BEGIN_COMPONENT(CGqlDataModelBridgeComp);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionClient);
		I_ASSIGN(m_gqlClientCompPtr, "GqlClient",
				"Asynchronous GraphQL client used to send queries and mutations", true, "GqlClient");
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager",
				"Subscription manager providing live updates over WebSocket, optional", false, "SubscriptionManager");
		I_ASSIGN(m_queryCommandIdAttrPtr, "QueryCommandId",
				"GraphQL query command used by GetModel (e.g. 'GetUser')", true, "");
		I_ASSIGN(m_mutationCommandIdAttrPtr, "MutationCommandId",
				"GraphQL mutation command used by SetModel (e.g. 'SetUser'), optional for read-only models", false, "");
		I_ASSIGN(m_subscriptionCommandIdAttrPtr, "SubscriptionCommandId",
				"GraphQL subscription command used by SubscribeModel (e.g. 'UserChanged'), optional", false, "");
		I_ASSIGN_MULTI_0(m_fieldsAttrPtr, "Fields",
				"Payload fields selected by the query / subscription; nested objects via dotted paths (e.g. 'address.city')", true);
		I_ASSIGN_MULTI_0(m_mutationFieldsAttrPtr, "MutationFields",
				"Payload fields selected by the mutation, optional (default: 'success')", false);
	I_END_COMPONENT;

	// reimplemented (IDataModelBridge)
	virtual void GetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			GetModelCallback callback) override;

	virtual void SetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			const QVariant& model,
			SetModelCallback callback) override;

	virtual int SubscribeModel(
			const QString& modelId,
			const QVariantMap& parameters,
			ModelUpdateCallback callback) override;

	virtual void UnsubscribeModel(int subscriptionId) override;

protected:
	// reimplemented (imtclientgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(
			const QByteArray& subscriptionId,
			const SubscriptionStatus& status,
			const QString& message) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private:
	struct Subscription
	{
		QByteArray wireSubscriptionId;
		ModelUpdateCallback callback;
	};

	/**
		\brief Signature identifying an upstream (wire) subscription:
		identical parameters reuse one registration at the
		subscription manager.
	*/
	static QByteArray CreateSubscriptionSignature(
			const QByteArray& commandId,
			const QVariantMap& parameters);

	/**
		\brief Fill \a request: the non-empty entries of
		\a inputValues become the \c input argument object,
		\a fieldPaths the payload selection set.
	*/
	void SetupRequest(
			imtgql::CGqlRequest& request,
			const QVariantMap& inputValues,
			const QByteArrayList& fieldPaths) const;

	QByteArrayList GetQueryFieldPaths() const;
	QByteArrayList GetMutationFieldPaths() const;

	/**
		\brief Extract the object below \a commandId from the raw
		GraphQL response JSON in \a responseData.
		\returns \c false and fills \a errorMessage if the response
		carries GraphQL errors or cannot be decoded.
	*/
	static bool DecodePayload(
			const QByteArray& responseData,
			const QByteArray& commandId,
			QVariantMap& payload,
			QString& errorMessage);

private:
	I_REF(imtclientgql::IAsyncGqlClient, m_gqlClientCompPtr);
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_ATTR(QByteArray, m_queryCommandIdAttrPtr);
	I_ATTR(QByteArray, m_mutationCommandIdAttrPtr);
	I_ATTR(QByteArray, m_subscriptionCommandIdAttrPtr);
	I_MULTIATTR(QByteArray, m_fieldsAttrPtr);
	I_MULTIATTR(QByteArray, m_mutationFieldsAttrPtr);

	mutable QMutex m_subscriptionsMutex;
	QMap<int, Subscription> m_subscriptions;
	QMap<QByteArray, QByteArray> m_wireSubscriptionIds; // signature -> wire subscription ID
	int m_nextSubscriptionId = 1;
};


} // namespace imtqml
