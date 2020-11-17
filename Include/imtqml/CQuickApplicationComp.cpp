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


#include <imtqml/CQuickApplicationComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QtGlobal>
#include <QtGui/QIcon>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuickWidgets/QQuickWidget>
#include <QQuickView>
//#if QT_VERSION >= 0x050000
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QVBoxLayout>
//#else
//#include <QtGui/QApplication>
//#include <QtGui/QVBoxLayout>
//#endif

#if defined (Q_OS_WIN)
	#if QT_VERSION >= 0x050500
	#include <QtPlatformHeaders/QWindowsWindowFunctions>
	#endif
#endif

// ACF includes
#include <istd/CSystem.h>
#include <iqtgui/CCommandTools.h>
#include <iqtgui/CHierarchicalCommand.h>


#if defined(Q_OS_MACX) && (QT_VERSION >= 0x040000) && (QT_VERSION < 0x050000)
void qt_mac_set_menubar_icons(bool enable);
#endif


namespace imtqml
{


// public methods

CQuickApplicationComp::CQuickApplicationComp()
	:m_defaultWidgetFlags(0),
	m_trayMessages(*this)
{
}


// reimplemented (imtgui::IQuickApplication)

const imtqml::IQuickObject* CQuickApplicationComp::GetApplicationItem() const
{
    if (m_mainQuickCompPtr.IsValid()){
        return m_mainQuickCompPtr.GetPtr();
    }

    return NULL;
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

     //Q_INIT_RESOURCE(imtgui);

	if (BaseClass::InitializeApplication(argc, argv)){
		m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_STARTING);

//		TryShowSplashScreen();

//        QQmlEngine engine;
        QQmlApplicationEngine engine;
        engine.addImportPath("qrc:/");
        engine.addImportPath("qrc:/qml");
        engine.load(QUrl("qrc:/qml/MainWindow.qml"));
//        QQuickView *view = new QQuickView;
//        view->setSource(QUrl("qrc:/qml/MainWindow.qml"));
//        view->show();

		if (m_allowApplicationCloseModelCompPtr.IsValid()){
			m_allowApplicationCloseModelCompPtr->AttachObserver(this);
		}

        if (m_mainQuickCompPtr.IsValid()){
//            m_mainQuickCompPtr.GetPtr()->CreateItem(&engine);


//			if (m_frameSpaceSizeAttrPtr.IsValid()){
//                m_mainWindowPtr.SetPtr(new QWidget());

//                QVBoxLayout* frameLayout = new QVBoxLayout(m_mainWindowPtr.GetPtr());

//				frameLayout->setMargin(*m_frameSpaceSizeAttrPtr);

//				// Create application's main widget:
//                m_mainGuiCompPtr->CreateGui(m_mainWindowPtr.GetPtr());
//			}
//			else{
//				m_mainGuiCompPtr->CreateGui(NULL);

//                m_mainWindowPtr.SetPtr(m_mainGuiCompPtr->GetWidget());
//			}


//			QString operationSystemName = istd::CSystem::GetOperationSystemName();
//            m_mainWindowPtr->setProperty("OperatingSystem", operationSystemName);

//#if defined(Q_OS_OSX)
//			m_mainWidgetPtr->setAttribute(Qt::WA_MacNormalSize, true);
//#endif

//            m_mainWindowPtr->setWindowTitle(QCoreApplication::applicationName());
//            m_mainWindowPtr->setWindowIcon(QApplication::windowIcon());
		}

		HideSplashScreen();

		if (*m_useTrayIconAttrPtr){
			m_trayIconPtr.SetPtr(new QSystemTrayIcon);
#if QT_VERSION >= 0x050000
			m_trayIconPtr->setIcon(QGuiApplication::windowIcon());
#else
			m_trayIconPtr->setIcon(QApplication::windowIcon());
#endif

			QMenu* trayMenuPtr = new QMenu;

			if (m_trayIconCommandsCompPtr.IsValid()){
				const iqtgui::CHierarchicalCommand* commandPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(m_trayIconCommandsCompPtr->GetCommands());
				if (commandPtr != NULL){
					iqtgui::CCommandTools::CreateMenu<QMenu>(*commandPtr, *trayMenuPtr);
				}
			}

			trayMenuPtr->addSeparator();

			trayMenuPtr->addAction(tr("Quit"), this, SLOT(OnQuit()));

			m_trayIconPtr->setContextMenu(trayMenuPtr);

			m_trayIconPtr->setVisible(true);
		}

//        if (m_mainWindowPtr.IsValid()){
//            m_defaultWidgetFlags = m_mainWindowPtr->windowFlags();

//			UpdateMainWidgetDecorations();

//			ShowWindow();

//            m_lastWidgetGeometry = m_mainWindowPtr->geometry();

//			QTimer::singleShot(0, this, SLOT(OnEventLoopStarted()));

//			// Start application loop:
//			retVal = QApplication::exec();

//			m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_SHUTDOWN);

//			Q_ASSERT(m_mainGuiCompPtr.IsValid());
//			m_mainGuiCompPtr->DestroyGui();

//            m_mainWindowPtr.Reset();
//		}
//		else{
//			if (m_trayIconPtr.IsValid()){
				QTimer::singleShot(0, this, SLOT(OnEventLoopStarted()));

				// Start application loop:
                retVal = QGuiApplication::exec();

				m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_SHUTDOWN);
//			}
//		}
	}

	if (m_trayIconPtr.IsValid()){
		QMenu* trayMenuPtr = m_trayIconPtr->contextMenu();
		if (trayMenuPtr != NULL){
			delete trayMenuPtr;
		}
	}

	m_trayIconPtr.Reset();

	return retVal;
}


QString CQuickApplicationComp::GetHelpText() const
{
	return "-style QtStyle\tname of Qt-specified style";
}


// protected methods

// reimplemented (imod::TSingleModelObserverBase)

void CQuickApplicationComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
//    if (m_mainWindowPtr.IsValid()){
//        m_lastWidgetGeometry = m_mainWindowPtr->geometry();

//		UpdateMainWidgetDecorations();

//		ShowWindow();

//        if (		!m_mainWindowPtr->isMaximized() &&
//                    !m_mainWindowPtr->isMinimized() &&
//                    !m_mainWindowPtr->isFullScreen()){
//            m_mainWindowPtr->setGeometry(m_lastWidgetGeometry);
//		}
//	}
}


// reimplemented (icomp::CComponentBase)

void CQuickApplicationComp::OnComponentDestroyed()
{
	BaseClass2::EnsureModelDetached();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CQuickApplicationComp::UpdateMainWidgetDecorations()
{
//    if (m_allowApplicationCloseCompPtr.IsValid() && m_mainWindowPtr.IsValid()){
//		Qt::WindowFlags windowFlags = m_defaultWidgetFlags;

//		if (!m_allowApplicationCloseCompPtr->IsEnabled()){
//            windowFlags = (m_mainWindowPtr->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint;
//		}

//        if (windowFlags != m_mainWindowPtr->windowFlags()){
//            m_mainWindowPtr->setWindowFlags(windowFlags);
//		}
//	}
}


void CQuickApplicationComp::ShowWindow()
{
//    Q_ASSERT(m_mainWindowPtr.IsValid());

//	int uiStartMode = 0;
//	if (m_uiStartModeAttrPtr.IsValid()){
//		uiStartMode = *m_uiStartModeAttrPtr;
//	}

//	bool usedFullscreenBorder = false;

//#if defined (Q_OS_WIN)
//	#if QT_VERSION >= 0x050500
//		usedFullscreenBorder = *m_useFullScreenBorderOnWindowsAttrPtr;
//	#endif
//#endif
//	QWindow* windowHandle = NULL;

//	switch (uiStartMode){
//		case 1:
//#if QT_VERSION >= 0x050500
//			windowHandle = m_mainWidgetPtr->windowHandle();
//#if defined (Q_OS_WIN)
//			if (windowHandle != NULL){
//				QWindowsWindowFunctions::setHasBorderInFullScreen(windowHandle, usedFullscreenBorder);
//			}
//#endif
//#if QT_VERSION >= 0x050000
//			// workaround to go full screen after start (Windows, Qt 5.6 - 5.10)
//			m_mainWidgetPtr->showMaximized();
//#endif
//#endif
//            m_mainWindowPtr->showFullScreen();
//			break;

//		case 2:
//            m_mainWindowPtr->showMinimized();
//			break;

//		case 3:
//            m_mainWindowPtr->showMaximized();
//			break;

//		default:
//            m_mainWindowPtr->show();
//	}
}


void CQuickApplicationComp::OnEventLoopStarted()
{
	m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_RUNNING);

	InitializeComponents();
	
	Q_ASSERT(qApp->startingUp() == false);
}


// private slots

void CQuickApplicationComp::OnQuit()
{
//    if (m_mainWindowPtr.IsValid()){
//        m_mainWindowPtr->close();
//	}
//	else{
//		QCoreApplication::quit();
//	}

    QCoreApplication::quit();

}


// public methods of the embedded class TrayMessages

CQuickApplicationComp::TrayMessages::TrayMessages(CQuickApplicationComp& parent)
	:m_parent(parent)
{
}


// reimplemented (ilog::IMessageConsumer)

bool CQuickApplicationComp::TrayMessages::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return m_parent.m_trayIconPtr.IsValid();
}


void CQuickApplicationComp::TrayMessages::AddMessage(const MessagePtr& messagePtr)
{
	if (m_parent.m_trayIconPtr.IsValid()){
		QSystemTrayIcon::MessageIcon severityIcon = QSystemTrayIcon::NoIcon;

		switch (messagePtr->GetInformationCategory()){
		case istd::IInformationProvider::IC_INFO:
			severityIcon = QSystemTrayIcon::Information;
			break;

		case istd::IInformationProvider::IC_WARNING:
			severityIcon = QSystemTrayIcon::Warning;
			break;

		case istd::IInformationProvider::IC_ERROR:
		case istd::IInformationProvider::IC_CRITICAL:
			severityIcon = QSystemTrayIcon::Critical;
			break;

		default:
			break;
		}

		m_parent.m_trayIconPtr->showMessage(QCoreApplication::applicationName(), messagePtr->GetInformationDescription(), severityIcon);
	}
}


} // namespace imtqml


