#include <imtqml/CQuickApplicationCompBase.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui/QGuiApplication>


namespace imtqml
{


static int s_argc = 0;


CQuickApplicationCompBase::CQuickApplicationCompBase()
:	m_useSplashScreen(false)
{
}


// reimplemented (ibase::IApplication)

bool CQuickApplicationCompBase::InitializeApplication(int argc, char** argv)
{
	m_applicationArguments.clear();

	// parse arguments:
	for (int argIndex = 0; argIndex < argc; argIndex++){
		QByteArray arg = argv[argIndex];

		if (argIndex + 1 < argc){	// two argument parameters
			if (arg == "-style"){
				m_appStyle = argv[++argIndex];
				continue;
			}

			if (arg == "-stylesheet"){
				m_styleSheetFile = QString::fromLocal8Bit(argv[++argIndex]);
				continue;
			}
		}

		m_applicationArguments << QString::fromLocal8Bit(argv[argIndex]);

		qDebug(argv[argIndex]);
	}

	if (!m_applicationPtr.IsValid()){
		s_argc = argc;
        m_applicationPtr.SetPtr(new QGuiApplication(s_argc, argv));
		if (!m_applicationPtr.IsValid()){
			return false;
		}

		if (IsComponentActive()){
			InitializeComponentApplication();
		}
	}

	return true;
}


QStringList CQuickApplicationCompBase::GetApplicationArguments() const
{
	return m_applicationArguments;
}


// protected methods

QGuiApplication* CQuickApplicationCompBase::GetQtApplication() const
{
	return m_applicationPtr.GetPtr();
}


bool CQuickApplicationCompBase::TryShowSplashScreen()
{
	m_splashScreenTimer.Start();
	m_useSplashScreen = m_splashScreenCompPtr.IsValid() && m_splashScreenCompPtr->CreateGui(NULL);
	if (m_useSplashScreen){
		QWidget* splashWidgetPtr = m_splashScreenCompPtr->GetWidget();
		Q_ASSERT(splashWidgetPtr != NULL);

		splashWidgetPtr->show();
		splashWidgetPtr->raise();

        QEventLoop loop;
#ifdef Q_OS_LINUX
		QTimer initSplashTimer;
		QObject::connect(&initSplashTimer, SIGNAL(timeout()), &loop, SLOT(quit()), Qt::QueuedConnection);
		Q_EMIT initSplashTimer.start(1000);
		loop.exec(QEventLoop::ExcludeUserInputEvents);
#else
		m_applicationPtr->processEvents(QEventLoop::ExcludeUserInputEvents);
#endif
		QTimer::singleShot(*m_splashTimeAttrPtr * 1000, &loop, SLOT(quit()));
		loop.exec();

		return true;
	}

	return false;
}


void CQuickApplicationCompBase::HideSplashScreen()
{
	if (m_useSplashScreen){
		Q_ASSERT(m_splashScreenCompPtr.IsValid());

		Q_ASSERT(m_splashTimeAttrPtr.IsValid());
		m_splashScreenTimer.WaitTo(m_splashTimeAttrPtr->GetValue());

		QWidget* splashWidgetPtr = m_splashScreenCompPtr->GetWidget();
		Q_ASSERT(splashWidgetPtr != NULL);

		splashWidgetPtr->hide();

		m_splashScreenCompPtr->DestroyGui();
	}
}


void CQuickApplicationCompBase::InitializeComponentApplication()
{
	Q_ASSERT(IsComponentActive());
	Q_ASSERT(m_applicationPtr.IsValid());

	if (m_styleSheetFile.isEmpty() && m_styleSheetAttrPtr.IsValid()){
		m_styleSheetFile = *m_styleSheetAttrPtr;
	}

	if (!m_appStyle.isEmpty()){
//		QApplication::setStyle(m_appStyle);
	}

	ApplyStyleSheet();

	// set up application name
	QString applicationFullName;
	if (m_applicationInfoCompPtr.IsValid()){
		applicationFullName = *m_titleFormatAttrPtr;

		applicationFullName.replace("$(CompanyName)", m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_COMPANY_NAME));
		applicationFullName.replace("$(ProductName)", m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_PRODUCT_NAME));
		applicationFullName.replace("$(AppName)", m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_NAME));
		applicationFullName.replace("$(AppSubName)", m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_SUBNAME));
		applicationFullName.replace("$(AppType)", m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_TYPE));
		applicationFullName.replace("$(Version)", m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_MAIN_VERSION));
	}
	else{
		applicationFullName = QObject::tr("ACF application");
	}
	m_applicationPtr->setApplicationName(applicationFullName);

	// set up icon
	QIcon icon;
	if (m_iconPathAttrPtr.IsValid()){
		icon = QIcon(*m_iconPathAttrPtr);
	}
	else{
		icon.addFile(":/Icons/AcfLogo");
	}
	m_applicationPtr->setWindowIcon(icon);

	if (m_applicationInfoCompPtr.IsValid()){
		// set up application data
		QString mainVersionText = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_MAIN_VERSION, false);
		m_applicationPtr->setApplicationVersion(mainVersionText);

		QString companyNameText = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_COMPANY_NAME, false);
		m_applicationPtr->setOrganizationName(companyNameText);
	}

	// set up current language
	if (m_translationManagerCompPtr.IsValid() && (m_translationManagerCompPtr->GetCurrentLanguageIndex() < 0)){
		m_translationManagerCompPtr->SetSystemLanguage();
	}
}


void CQuickApplicationCompBase::ApplyStyleSheet()
{
	// Set up style sheet:
	if (!m_styleSheetFile.isEmpty()){
//		if (!iqtgui::SetStyleSheetFromFile(*m_applicationPtr, m_styleSheetFile)){
//			qDebug("Style sheet file could not be set: %s", m_styleSheetFile.toLocal8Bit().constData());
//		}
	}
	else{
		// Reset style sheet:
//		m_applicationPtr->setStyleSheet("");
	}
}


void CQuickApplicationCompBase::InitializeComponents()
{
	for (int i = 0; i < m_componentsToInitializeCompPtr.GetCount(); ++i){
		istd::IPolymorphic* componentPtr = m_componentsToInitializeCompPtr[i];
		Q_UNUSED(componentPtr);
	}
}


// reimplemented (icomp::CComponentBase)

void CQuickApplicationCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_applicationPtr.IsValid()){
		InitializeComponentApplication();
	}
}


// public methods of the embedded class RuntimeStatus

CQuickApplicationCompBase::RuntimeStatus::RuntimeStatus()
	:m_status(RS_NONE)
{
}


void CQuickApplicationCompBase::RuntimeStatus::SetRuntimeStatus(IRuntimeStatusProvider::RuntimeStatus runtimeStatus)
{
	if (m_status != runtimeStatus){
		istd::CChangeNotifier changePtr(this);

		m_status = runtimeStatus;
	}
}


// reimplemented (ibase::IRuntimeStatusProvider)

ibase::IRuntimeStatusProvider::RuntimeStatus CQuickApplicationCompBase::RuntimeStatus::GetRuntimeStatus() const
{
	return m_status;
}


} // namespace imtqml



