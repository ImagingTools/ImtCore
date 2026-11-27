// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQuickApplicationComp.h>


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QtMath>
#include <QtCore/QSettings>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlComponent>

// ACF includes
#include <istd/TDelPtr.h>


#if defined(Q_OS_MACX) && (QT_VERSION >= 0x040000) && (QT_VERSION < 0x050000)
void qt_mac_set_menubar_icons(bool enable);
#endif


namespace imtqml
{


const QByteArray CQuickApplicationComp::s_settingsGroup = QByteArrayLiteral("MainWindow");


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

			// Fixed-size splash window, shown while the main QML tree is being created:
			imtqml::IQuickObject* splashQuickPtr = m_splashCompPtr.GetPtr();
			bool isCustomSplash = false;
			QQmlComponent hostComponent(engine.GetPtr(), QUrl("qrc:/imtqml/SplashHostWindow.qml"));
			QQuickWindow* hostWindowPtr = qobject_cast<QQuickWindow*>(hostComponent.create());
			if (hostWindowPtr != nullptr){
				QQuickItem* splashItemPtr = nullptr;
				if (splashQuickPtr != nullptr){
					if (splashQuickPtr->CreateQuickItem(hostWindowPtr->contentItem()) && splashQuickPtr->GetQuickItem() != nullptr){
						splashItemPtr = splashQuickPtr->GetQuickItem();
						isCustomSplash = true;
					}
					else{
						qWarning() << "Unable to create the splash item";
					}
				}

				if (splashItemPtr == nullptr){
					QQmlComponent defaultSplashComponent(engine.GetPtr(), QUrl("qrc:/imtqml/DefaultSplashScreen.qml"));
					splashItemPtr = qobject_cast<QQuickItem*>(defaultSplashComponent.create());
					if (splashItemPtr == nullptr){
						qWarning() << "Unable to create the default splash item:" << defaultSplashComponent.errors();
					}
					else{
						splashItemPtr->setParent(hostWindowPtr);
						splashItemPtr->setParentItem(hostWindowPtr->contentItem());
					}
				}

				if (splashItemPtr != nullptr){
					if (splashItemPtr->implicitWidth() > 0 && splashItemPtr->implicitHeight() > 0){
						hostWindowPtr->resize(qCeil(splashItemPtr->implicitWidth()), qCeil(splashItemPtr->implicitHeight()));
					}
					splashItemPtr->setSize(QSizeF(hostWindowPtr->width(), hostWindowPtr->height()));
					hostWindowPtr->show();
				}
			}

			auto releaseSplash = [&](){
				if (isCustomSplash){
					splashQuickPtr->DestroyQuickItem();
				}
				delete hostWindowPtr;
				hostWindowPtr = nullptr;
			};
			QGuiApplication::processEvents();

			QQmlComponent mainComponent(engine.GetPtr(), QUrl("qrc:/qml/MainWindow.qml"), QQmlComponent::Asynchronous);
			QEventLoop loop;
			if (mainComponent.isLoading()){
				QObject::connect(&mainComponent, &QQmlComponent::statusChanged, &loop, &QEventLoop::quit);
				loop.exec();
			}

			QObject* root = nullptr;
			CLoopIncubator incubator(loop);
			if (mainComponent.isReady()){
				mainComponent.create(incubator);
				if (incubator.isLoading()){
					loop.exec();
				}
				root = incubator.object();
			}
			if (root != nullptr){
				root->setParent(engine.GetPtr());
			}

			imtqml::IQuickObject* quickObjectPtr = m_mainQuickCompPtr.GetPtr();
			if (quickObjectPtr == nullptr){
				releaseSplash();
				return -1;
			}

			if (root != nullptr){
				QQuickWindow* window = qobject_cast<QQuickWindow*>(root);
				if (window != nullptr){
					window->setOpacity(0);
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
					window->setWindowState(Qt::WindowMaximized);
					window->setIcon(QGuiApplication::windowIcon());
					window->setTitle(QGuiApplication::applicationName());

					// Apply saved window geometry:
					QSettings settings;
					settings.beginGroup(QString::fromLatin1(s_settingsGroup));
					if (settings.contains("geometry")){
						const QRect geometry = settings.value("geometry").toRect();
						window->setWindowState(Qt::WindowNoState);
						if (geometry.isValid()){
							window->setGeometry(geometry);
						}
						if (settings.value("maximized", true).toBool()){
							window->setWindowState(Qt::WindowMaximized);
						}
					}
					settings.endGroup();

					QObject::connect(qApp, &QCoreApplication::aboutToQuit, window, [window](){
						QSettings settings;
						settings.beginGroup(QString::fromLatin1(s_settingsGroup));
						const bool maximized = window->windowState() & Qt::WindowMaximized;
						settings.setValue("maximized", maximized);
						if (!maximized){
							settings.setValue("geometry", window->geometry());
						}
						settings.endGroup();
					});

					window->show();
					QGuiApplication::processEvents();

					QQuickItem* mainItem = window->contentItem();
					if (mainItem != nullptr){
						quickObjectPtr->CreateQuickItem(mainItem);
					}
					QGuiApplication::processEvents();
					window->setOpacity(1);
					window->requestUpdate();
				}
			}

			releaseSplash();
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


