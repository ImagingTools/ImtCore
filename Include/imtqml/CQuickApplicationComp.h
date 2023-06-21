#pragma once


// Qt includes
#include <QtWidgets/QSystemTrayIcon>

// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iprm/IEnableableParam.h>
#include <ilog/IMessageConsumer.h>
#include <ibase/ICommandsProvider.h>
#include <icomp/IComponent.h>

// ImtCore includes
#include <imtqml/CQuickApplicationCompBase.h>
#include <imtqml/IQuickObject.h>
#include <imtqml/IQuickApplication.h>


namespace imtqml
{


class CQuickApplicationComp:
			public QObject,
			public CQuickApplicationCompBase
{
	Q_OBJECT
public:
	typedef imtqml::CQuickApplicationCompBase BaseClass;

	I_BEGIN_COMPONENT(CQuickApplicationComp);
		I_REGISTER_INTERFACE(ibase::IApplication);
		I_ASSIGN(m_mainQuickCompPtr, "MainQuickWindow", "QuickWindow object shown as main window", false, "MainGui");
		I_ASSIGN(m_contextCompPtr, "Context", "Client specific context", false, "Context");
	I_END_COMPONENT;

	CQuickApplicationComp();

	// reimplemented (ibase::IGuiApplication)
	virtual const imtqml::IQuickObject* GetApplicationItem() const;

	// reimplemented (ibase::IApplication)
	virtual bool InitializeApplication(int argc, char** argv);
	virtual int Execute(int argc, char** argv);
	virtual QString GetHelpText() const;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed();

private:
	void UpdateMainWidgetDecorations();
	void ShowWindow();

private Q_SLOTS:
	void OnQuit();
	void OnEventLoopStarted();

protected:
	I_REF(imtqml::IQuickObject, m_mainQuickCompPtr);
	I_REF(icomp::IComponent, m_contextCompPtr);
};


} // namespace imtqml


