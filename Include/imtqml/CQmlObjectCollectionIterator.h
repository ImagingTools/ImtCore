#pragma once


#include <QObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IOptionsList.h>

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
	Q_INVOKABLE virtual QString getData(QString fieldName);


private:
    imtbase::IObjectCollectionIterator *m_objectCollectionIterator;

};


}



