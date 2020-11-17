#pragma once


// Qt includes
#include <QtGui/QGuiApplication>

// ACF includes
#include <istd/CGeneralTimeStamp.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>
#include <ibase/IApplication.h>
#include <ibase/IApplicationInfo.h>
#include <ibase/IRuntimeStatusProvider.h>
#include <iqt/ITranslationManager.h>
#include <iqtgui/IGuiObject.h>

// ImtCore includes
#include <imtqml/IQuickObject.h>
#include <imtqml/IQuickApplication.h>


namespace imtqml
{


class CQuickApplicationCompBase:
			public icomp::CComponentBase, 
			virtual public ibase::IApplication
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CQuickApplicationCompBase);
		I_REGISTER_INTERFACE(ibase::IApplication);
		I_REGISTER_SUBELEMENT(RuntimeStatus);
		I_REGISTER_SUBELEMENT_INTERFACE(RuntimeStatus, ibase::IRuntimeStatusProvider, ExtractRuntimeStatus);
		I_REGISTER_SUBELEMENT_INTERFACE(RuntimeStatus, imod::IModel, ExtractRuntimeStatus);
		I_REGISTER_SUBELEMENT_INTERFACE(RuntimeStatus, istd::IChangeable, ExtractRuntimeStatus);
		I_ASSIGN(m_splashScreenCompPtr, "SplashScreen", "Splash screen shown before application is launched", false, "SplashScreen");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info used to set main window title", false, "ApplicationInfo");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_splashTimeAttrPtr, "SplashTime", "Minimal time splash screen will be shown", true, 2);
		I_ASSIGN(m_styleSheetAttrPtr, "StyleSheet", "Style sheet for the GUI", false, "");
		I_ASSIGN(m_iconPathAttrPtr, "IconPath", "file path for the application icon", true, ":/Icons/AcfLogo");
		I_ASSIGN(m_titleFormatAttrPtr, "TitleFormat", "Describe format of title bar, tags:\n\t$(CompanyName) - name of company\n\t$(ProductName) - product name\n\t$(AppName) - application name\n\t$(AppSubName) - application sub name\n\t$(AppType) - type of application\n\t$(Version) - main application version", true, "$(AppName)");
		I_ASSIGN_MULTI_0(m_componentsToInitializeCompPtr, "ComponentsToInitialize", "List of components to be initialized after creation of the application instance (QCoreApplication)", false);
	I_END_COMPONENT;

	CQuickApplicationCompBase();

	// reimplemented (ibase::IApplication)
	virtual bool InitializeApplication(int argc, char** argv);
	virtual QStringList GetApplicationArguments() const;

protected:
	QGuiApplication* GetQtApplication() const;
	bool TryShowSplashScreen();
	void HideSplashScreen();
	void InitializeComponentApplication();
	void ApplyStyleSheet();
	void InitializeComponents();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	class RuntimeStatus : public ibase::IRuntimeStatusProvider
	{
	public:
		RuntimeStatus();

		void SetRuntimeStatus(IRuntimeStatusProvider::RuntimeStatus runtimeStatus);

		// reimplemented (ibase::IRuntimeStatusProvider)
		virtual IRuntimeStatusProvider::RuntimeStatus GetRuntimeStatus() const;

	private:
		IRuntimeStatusProvider::RuntimeStatus m_status;
	};

	// static template methods for sub element access
	template <class InterfaceType>
	static InterfaceType* ExtractRuntimeStatus(CQuickApplicationCompBase& component)
	{
		return &component.m_runtimeStatus;
	}

protected:
	imod::TModelWrap<RuntimeStatus> m_runtimeStatus;

private:
	I_REF(iqtgui::IGuiObject, m_splashScreenCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_ATTR(double, m_splashTimeAttrPtr);
	I_ATTR(QString, m_styleSheetAttrPtr);
	I_ATTR(QString, m_iconPathAttrPtr);
	I_ATTR(QString, m_titleFormatAttrPtr);
	I_MULTIREF(istd::IPolymorphic, m_componentsToInitializeCompPtr);

	bool m_useSplashScreen;
	istd::CGeneralTimeStamp m_splashScreenTimer;

	istd::TDelPtr<QGuiApplication> m_applicationPtr;

	QStringList m_applicationArguments;

	QByteArray m_appStyle;
	QString m_styleSheetFile;
};


} // namespace imtqml


