// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtservergql/CCollectionDocumentServicePublisherComp.h>


/**
	Publisher that keeps the pushed notifications instead of sending them to a socket.
*/
class CTestableCollectionDocumentServicePublisher: public imtservergql::CCollectionDocumentServicePublisherComp
{
public:
	typedef imtservergql::CCollectionDocumentServicePublisherComp BaseClass;

	I_BEGIN_COMPONENT(CTestableCollectionDocumentServicePublisher);
	I_END_COMPONENT;

	struct PushedData
	{
		QByteArray commandId;
		QByteArray data;
	};

	void AddSubscriber(const QByteArray& subscriptionId, const QByteArray& commandId, const imtrest::IRequest& networkRequest);

	QList<PushedData> m_pushedData;

protected:
	// reimplemented (imtservergql::CGqlPublisherCompBase)
	virtual bool PushDataToSubscriber(
				const QByteArray& subscriptionId,
				const QByteArray& commandId,
				const QByteArray& data,
				const imtrest::IRequest& networkRequest,
				const bool useAwsStyle = true) const override;
};


/**
	The DocumentChanged notification names the request that made the change: the X-Request-ID header of the request context
	of the thread the change is published on.
*/
class CCollectionDocumentServicePublisherTest: public QObject
{
	Q_OBJECT

private slots:
	void cleanup();

	void RequestIdOfTheRequestContextTest();
	void NoRequestContextTest();
	void RequestContextWithoutRequestIdTest();
	void InvalidRequestIdTest();
};
