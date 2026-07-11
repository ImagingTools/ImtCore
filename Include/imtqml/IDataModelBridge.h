// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <functional>

// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>


namespace imtqml
{


/**
	\brief Transport-agnostic abstract bridge used by
	\c CDataModelController to fetch / store generic data models.

	\details
	A concrete bridge knows how to map a \c modelId (plus an opaque
	\c parameters map) to an actual fetch / store operation. Two
	implementations are provided out of the box:

	- \c CDataModelBridgeDemultiplexer: a demultiplexer that owns N delegate
	  bridges (also \c IDataModelBridge instances) connected via
	  \c I_MULTIREF and routes each call to the first delegate that
	  reports \c IsSupported for the given \c modelId — one bridge for
	  N model controllers in the in-process scenario.
	- \c CDataModelBridgeBase: a ready-to-extend base component
	  implementing \c IDataModelBridge with a \c ModelId attribute
	  and a default \c IsSupported that compares the requested
	  \c modelId against the attribute value — concrete per-model
	  bridges should inherit from this class and implement
	  \c GetModel / \c SetModel.

	All operations are asynchronous; implementations MUST invoke the
	callback exactly once on the GUI thread. An empty \c errorMessage
	signals success.
*/
class IDataModelBridge: virtual public istd::IPolymorphic
{
public:
	typedef std::function<void(QVariant /*model*/, QString /*errorMessage*/)> GetModelCallback;
	typedef std::function<void(QString /*errorMessage*/)> SetModelCallback;
	typedef std::function<void(QVariant /*model*/)> ModelUpdateCallback;

	/**
		\brief Returns \c true if this bridge is able to serve the
		model identified by \c modelId.

		\details
		Used by \c CDataModelBridgeDemultiplexer (demultiplexer) to pick the right
		delegate. The default implementation returns \c true so that
		bridges that can serve every \c modelId (e.g. a GraphQL bridge
		that simply forwards the request) do not need to override.
		Per-model delegates should override and compare \c modelId
		against the one(s) they own.
	*/
	virtual bool IsSupported(const QString& modelId) const
	{
		Q_UNUSED(modelId);
		return true;
	}

	virtual void GetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			GetModelCallback callback) = 0;

	virtual void SetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			const QVariant& model,
			SetModelCallback callback) = 0;

	/**
		\brief Subscribe for live updates of the model identified by
		\c modelId.

		\details
		Whenever the underlying data model changes, \c callback is
		invoked on the GUI thread with the fresh model value. The
		desktop bridge implements this via an in-process
		\c imod::IObserver; the GraphQL bridge via a WebSocket
		subscription. Returns an opaque subscription ID (> 0) to be
		passed to \c UnsubscribeModel, or 0 if live updates are not
		supported by this bridge (the default implementation).
	*/
	virtual int SubscribeModel(
			const QString& modelId,
			const QVariantMap& parameters,
			ModelUpdateCallback callback)
	{
		Q_UNUSED(modelId);
		Q_UNUSED(parameters);
		Q_UNUSED(callback);
		return 0;
	}

	/**
		\brief Cancel a live-update subscription created by
		\c SubscribeModel. Passing 0 or an unknown ID is a no-op.
	*/
	virtual void UnsubscribeModel(int subscriptionId)
	{
		Q_UNUSED(subscriptionId);
	}
};


} // namespace imtqml
