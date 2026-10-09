// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtQml/QQmlEngine>

// ImtCore includes
#include "CGqlTestTransport.h"


namespace imtqmltest
{


/**
	Qt Quick Test setup: makes the ImtCore QML modules importable from their resources
	and routes the network traffic of every test engine to CGqlTestTransport (QML singleton \c GqlTestTransport of the module \c imtqmltest).
*/
class CQmlTestSetup: public QObject
{
	Q_OBJECT

public:
	CQmlTestSetup();

public Q_SLOTS:
	void applicationAvailable();
	void qmlEngineAvailable(QQmlEngine* enginePtr);

private:
	CGqlTestTransport m_transport;
	CGqlTestNetworkAccessManagerFactory m_networkAccessManagerFactory;
};


} // namespace imtqmltest


