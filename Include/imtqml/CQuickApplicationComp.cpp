#include <imtqml/CQuickApplicationComp.h>

// Qt includes
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QtGlobal>
#include <QtGui/QIcon>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QJSEngine>
#include <QtQml/QQmlPropertyMap>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlContext>
#include <QtQuickWidgets/QQuickWidget>

#if defined (Q_OS_WIN)
	#if QT_VERSION >= 0x050500 && QT_VERSION < 0x060000
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

		QQmlApplicationEngine *engine = new QQmlApplicationEngine(this);
		engine->addImportPath("qrc:/");
		engine->addImportPath("qrc:/qml");
		engine->load(QUrl("qrc:/qml/MainWindow.qml"));

		if (m_allowApplicationCloseModelCompPtr.IsValid()){
			m_allowApplicationCloseModelCompPtr->AttachObserver(this);
		}

		if (m_mainQuickCompPtr.IsValid()){
			imtqml::IQuickObject *quickObjectPtr = m_mainQuickCompPtr.GetPtr();
			if (quickObjectPtr != nullptr){

				QList<QObject*> rootObjects = engine->rootObjects();
				if (!rootObjects.isEmpty()){
					QObject* root = rootObjects[0];
					Q_ASSERT(root != nullptr);

					QQuickWindow* window = qobject_cast<QQuickWindow*>(root);
					if (window != nullptr){
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

		QTimer::singleShot(0, this, SLOT(OnEventLoopStarted()));

		// Start application loop:
		retVal = QGuiApplication::exec();

		m_runtimeStatus.SetRuntimeStatus(ibase::IRuntimeStatusProvider::RS_SHUTDOWN);
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
}


void CQuickApplicationComp::ShowWindow()
{
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


