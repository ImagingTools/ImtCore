// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQuickApplicationComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>

// ACF includes
#include <istd/TDelPtr.h>


#if defined(Q_OS_MACX) && (QT_VERSION >= 0x040000) && (QT_VERSION < 0x050000)
void qt_mac_set_menubar_icons(bool enable);
#endif


namespace imtqml
{


// public methods

CQuickApplicationComp::CQuickApplicationComp()
{
}


// reimplemented (imtgui::IQuickApplication)

const imtqml::IQuickObject* CQuickApplicationComp::GetApplicationItem() const
{
	if (m_mainQuickCompPtr.IsValid()){
		return m_mainQuickCompPtr.GetPtr();
	}

	return nullptr;
}


// reimplemented (ibase::IApplication)

bool CQuickApplicationComp::InitializeApplication(int argc, char** argv)
{
#if QT_VERSION > 0x050000
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	qputenv("QT_HIDPI_AWARE", "1");
	qputenv("QT_HIGHDPI_AWARE", "1");
#endif

#if QT_VERSION > 0x050600
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#if defined(Q_OS_MACX) && (QT_VERSION >= 0x040000) && (QT_VERSION < 0x050000)
	QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, !*m_useMenuIconsOnMacAttrPtr);
	qt_mac_set_menubar_icons(*m_useMenuIconsOnMacAttrPtr);
#endif

	if (BaseClass::InitializeApplication(argc, argv)){
		return true;
	}

	return false;
}


int CQuickApplicationComp::Execute(int argc, char** argv)
{
	int retVal = -1;

	if (BaseClass::InitializeApplication(argc, argv)){
		m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_STARTING);
		istd::TDelPtr<QQmlApplicationEngine> engine = new QQmlApplicationEngine(this);
		if (m_mainQuickCompPtr.IsValid()){
			engine->addImportPath("qrc:/");
			engine->addImportPath("qrc:/qml");

			QObject::connect(engine.GetPtr(), &QQmlApplicationEngine::quit, &QGuiApplication::quit);
			engine->load(QUrl("qrc:/qml/MainWindow.qml"));

			imtqml::IQuickObject* quickObjectPtr = m_mainQuickCompPtr.GetPtr();
			if (quickObjectPtr != nullptr){
				QList<QObject*> rootObjects = engine->rootObjects();
				if (!rootObjects.isEmpty()){
					QObject* root = rootObjects[0];
					Q_ASSERT(root != nullptr);

					QQuickWindow* window = qobject_cast<QQuickWindow*>(root);
					if (window != nullptr){
						if (m_windowWidthAttrPtr.IsValid()){
							window->setWidth(*m_windowWidthAttrPtr);
						}
						if (m_windowHeightAttrPtr.IsValid()){
							window->setHeight(*m_windowHeightAttrPtr);
						}
						if (m_minimumWindowWidthAttrPtr.IsValid()){
							window->setMinimumWidth(*m_minimumWindowWidthAttrPtr);
						}
						if (m_minimumWindowHeightAttrPtr.IsValid()){
							window->setMinimumHeight(*m_minimumWindowHeightAttrPtr);
						}
						if (m_visibilityAttrPtr.IsValid()){
							window->setVisibility(QWindow::Visibility(*m_visibilityAttrPtr));
						}
						window->setWindowState(Qt::WindowMaximized); // Qt::WindowActive
						window->setIcon(QGuiApplication::windowIcon());
						window->setTitle(QGuiApplication::applicationName());
						QQuickItem* mainItem = window->contentItem();
						if (mainItem != nullptr){
							quickObjectPtr->CreateQuickItem(mainItem);
						}
					}
				}
			}
			else{
				return -1;
			}
		}

		HideSplashScreen();

		QTimer::singleShot(0, this, &CQuickApplicationComp::OnEventLoopStarted);

		// Start application loop:
		retVal = QGuiApplication::exec();

		m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_SHUTDOWN);
	}

	return retVal;
}


QString CQuickApplicationComp::GetHelpText() const
{
	return "-style QtStyle\tname of Qt-specified style";
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CQuickApplicationComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// private methods

void CQuickApplicationComp::OnEventLoopStarted()
{
	m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_RUNNING);

	InitializeComponents();
	
	Q_ASSERT(qApp->startingUp() == false);
}


// private slots

void CQuickApplicationComp::OnQuit()
{
	QCoreApplication::quit();
}


} // namespace imtqml


