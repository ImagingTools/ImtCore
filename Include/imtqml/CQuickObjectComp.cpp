/********************************************************************************
**
**	Copyright (C) 2007-2017 Witold Gantzke & Kirill Lepskiy
**
**	This file is part of the ACF Toolkit.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
** 	See http://www.ilena.org or write info@imagingtools.de for further
** 	information about the ACF.
**
********************************************************************************/


#include <imtqml/CQuickObjectComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#if QT_VERSION >= 0x050000
//#include <QtWidgets/QStyle>
//#include <QtWidgets/QVBoxLayout>
#else
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>
#endif

// ACF includes
#include <icomp/CCompositeComponent.h>


namespace imtqml
{

CQuickObjectComp::CQuickObjectComp()
:   m_quickItem(nullptr)
{

}


bool CQuickObjectComp::IsItemCreated() const
{
    return  m_quickItem != nullptr;
}


bool CQuickObjectComp::CreateItem(QQuickItem *parentPtr)
{
    if(parentPtr == nullptr || m_pathToQmlPtr.IsValid() == false)
        return false;
    QQmlEngine *engine = qmlEngine(parentPtr);
    CreateItem(engine);
    if(m_quickItem != nullptr)
        m_quickItem->setParentItem(parentPtr);
    return  IsItemCreated();
}


bool CQuickObjectComp::CreateItem(QQmlEngine *engine)
{
    if(m_pathToQmlPtr.IsValid() == false)
        return false;
    QQmlComponent component(engine, QUrl::fromLocalFile((m_pathToQmlPtr->GetValue())));
//    QQmlContext *context = new QQmlContext(engine->rootContext());
    m_quickItem = qobject_cast<QQuickItem*>(component.create(engine->rootContext()));
//    m_quickItem->setParent(engine->rootContext()->contextObject());
//    m_quickItem->setVisible(true);
//    QQuickItem *parentItem = qobject_cast<QQuickItem*>(engine->rootContext()->contextObject());
//    if(parentItem != nullptr)
//        m_quickItem->setParentItem(parentItem);
//    int width = m_quickItem->width();
    return  IsItemCreated();
}


bool CQuickObjectComp::DestroyItem()
{
    return false;
}


QQuickItem *CQuickObjectComp::GetItem() const
{
    return m_quickItem;
}


void CQuickObjectComp::OnTryClose(bool *ignoredPtr)
{

}

void CQuickObjectComp::OnComponentCreated()
{
    qDebug() << "create CQuickObjectComp Component";
}


} // namespace imtqml



