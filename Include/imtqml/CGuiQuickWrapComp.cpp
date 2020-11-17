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


#include <imtqml/CGuiQuickWrapComp.h>


// Qt includes
#include <QtGui/QGuiApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtQml/QQmlEngine>
#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>

// ACF includes
#include <icomp/CCompositeComponent.h>


namespace imtqml
{

CGuiQuickWrapComp::CGuiQuickWrapComp()
    : m_quickWidget(nullptr),
      m_layout(nullptr)
{

}

bool CGuiQuickWrapComp::IsGuiCreated() const
{
    return  m_quickWidget != nullptr;
}

bool CGuiQuickWrapComp::CreateGui(QWidget *parentPtr)
{
    if(m_quickCompPtr.IsValid()) {
        //Q_INIT_RESOURCE(imtgui);

		imtqml::IQuickObject *quickObject = m_quickCompPtr.GetPtr();
//        QQmlEngine *engine;

//        if(quickObject->IsItemCreated() == false) {
//            engine = new QQmlEngine();
////            quickObject->CreateItem(engine);
////            m_quickWidget = new QQuickWidget(engine, parentPtr);
//        }
//        else {
//            engine = qmlEngine(quickObject->GetItem());
////            quickObject->CreateItem(engine);
//        }
        if(m_quickWidget == nullptr) {
            m_quickWidget = new QQuickWidget(parentPtr);
            m_quickWidget->setSource(QUrl::fromLocalFile((":/qml/WrapItem.qml")));

//            QQmlEngine *engine = new QQmlEngine(m_quickWidget);
//             QQmlContext *context = new QQmlContext(engine->rootContext());
//             QQuickItem *rootWindow = qobject_cast<QQuickItem*>(m_quickWidget->rootObject());
//             int rootW = rootWindow->width();
//             context->setContextObject(&myDataSet);

//             QQmlComponent component(engine);
//             component.setData("import QtQuick 2.0\nRectangle { height: 200; width: 200; color: \"blue\" }", QUrl());
//             QQuickItem *window = qobject_cast<QQuickItem*>(component.create(context));
//             window->setParentItem(rootWindow);
//             int width = window->width();

//            quickObject->CreateItem(m_quickWidget->engine());
            quickObject->CreateItem(m_quickWidget->rootObject());
            QQuickItem *quickItem = quickObject->GetItem();
            if(quickItem != nullptr) {
//                quickItem->setParentItem(m_quickWidget->rootObject());
            }
            m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        }
//        m_quickWidget->setContent()

    }
    else {

    }

    if(parentPtr != nullptr) {
        m_layout = new QVBoxLayout(parentPtr);
        m_layout->addWidget(m_quickWidget);
    }
    return  m_quickWidget != nullptr;
}


bool CGuiQuickWrapComp::DestroyGui()
{
    if(m_quickWidget != nullptr)
        m_quickWidget->deleteLater();
    if(m_layout != nullptr)
        m_layout->deleteLater();
    return true;
}


QWidget *CGuiQuickWrapComp::GetWidget() const
{
    return m_quickWidget;
}

void CGuiQuickWrapComp::OnTryClose(bool *ignoredPtr)
{
    if(m_quickWidget != nullptr) {
        m_quickWidget->close();
    }
}

void CGuiQuickWrapComp::OnComponentCreated()
{
    qDebug() << "create CGuiQuickWrapComp Component";
}



} // namespace imtqml



