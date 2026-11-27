// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>


namespace imtassist
{


/**
	Context of one assistant request.

	User and tenant are derived server-side from the authenticated session by the
	transport layer. They are never taken from the model or from the client payload.
	The released context items are the only application context the user has
	explicitly shared with the assistant (for example the current selection).

	\ingroup imtassist
*/
class CAssistantRequestContext
{
public:
	/**
		Application context explicitly released by the user.
	*/
	struct ContextItem
	{
		/**
			Kind of the item, for example "selection".
		*/
		QByteArray kind;

		/**
			Text shown to the user, so that the shared context stays visible.
		*/
		QString label;

		/**
			Data of the item, passed to the provider as untrusted data.
		*/
		QJsonObject data;
	};

	typedef QList<ContextItem> ContextItems;

	CAssistantRequestContext() = default;
	CAssistantRequestContext(const QByteArray& userId, const QByteArray& tenantId);

	QByteArray GetUserId() const;
	void SetUserId(const QByteArray& userId);

	QByteArray GetTenantId() const;
	void SetTenantId(const QByteArray& tenantId);

	/**
		Correlation ID used in the audit log of this request.
	*/
	QByteArray GetCorrelationId() const;
	void SetCorrelationId(const QByteArray& correlationId);

	ContextItems GetContextItems() const;
	void SetContextItems(const ContextItems& items);

	/**
		Authenticated context of the transport layer, e.g. the GraphQL context of the request.
		Opaque for the assistant core and never passed to a provider. Actions of the transport
		layer use it to call the existing APIs of the application with the rights of the user,
		so that the assistant can do nothing the user could not do in the application.
	*/
	const istd::IChangeable* GetTransportContext() const;
	void SetTransportContext(const istd::IChangeableSharedPtr& transportContextPtr);

	/**
		A context is valid if it carries an authenticated user.
	*/
	bool IsValid() const;

	/**
		Check whether both contexts belong to the same user in the same tenant.
	*/
	bool IsSamePrincipal(const CAssistantRequestContext& other) const;

private:
	QByteArray m_userId;
	QByteArray m_tenantId;
	QByteArray m_correlationId;
	ContextItems m_contextItems;
	istd::IChangeableSharedPtr m_transportContextPtr;
};


} // namespace imtassist


