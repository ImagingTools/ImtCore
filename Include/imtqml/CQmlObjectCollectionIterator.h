// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>


namespace imtqml
{


class CQmlObjectCollectionIterator : public QObject
{
	Q_OBJECT

public:
	CQmlObjectCollectionIterator(imtbase::IObjectCollectionIterator* iterator);
	Q_INVOKABLE virtual bool next();
	Q_INVOKABLE virtual bool previous();
	Q_INVOKABLE virtual QString getData(const QByteArray& fieldName);

private:
	imtbase::IObjectCollectionIterator* m_objectCollectionIterator;
};


}


