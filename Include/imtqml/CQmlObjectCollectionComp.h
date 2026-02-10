// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtqml/CQmlObjectCollectionIterator.h>


namespace imtqml
{


class CQmlObjectCollectionComp:
			public QObject,
			public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQmlObjectCollectionComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
	I_END_COMPONENT;

	Q_INVOKABLE virtual int GetElementsCount(QString filterParams = "");
	Q_INVOKABLE virtual imtqml::CQmlObjectCollectionIterator* CreateObjectCollectionIterator(int offset = 0, int count = -1, QString selectionParams = "");

private:
	iprm::CParamsSet* GetFilters(QString filterParams);

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtqml


