// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtqml/CQuickApplicationCompBase.h>


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
		I_ASSIGN(m_windowWidthAttrPtr, "WindowWidth", "Window width", false, 800);
		I_ASSIGN(m_windowHeightAttrPtr, "WindowHeight", "Window height", false, 600);
		I_ASSIGN(m_minimumWindowWidthAttrPtr, "MinimumWindowWidth", "Minimum window width", false, 800);
		I_ASSIGN(m_minimumWindowHeightAttrPtr, "MinimumWindowHeight", "Minimum window height", false, 600);
		I_ASSIGN(m_visibilityAttrPtr, "Visibility", "The screen-occupation state of the window:\n0 - Hidden;\n1 - AutomaticVisibility;\n2 - Windowed;\n3 - Minimized;\n4 - Maximized;\n5 - FullScreen", false, 1);
	I_END_COMPONENT;

	CQuickApplicationComp();

	// reimplemented (ibase::IGuiApplication)
	virtual const imtqml::IQuickObject* GetApplicationItem() const;

	// reimplemented (ibase::IApplication)
	virtual bool InitializeApplication(int argc, char** argv) override;
	virtual int Execute(int argc, char** argv) override;
	virtual QString GetHelpText() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnQuit();
	void OnEventLoopStarted();

protected:
	I_REF(imtqml::IQuickObject, m_mainQuickCompPtr);
	I_ATTR(int, m_windowWidthAttrPtr);
	I_ATTR(int, m_windowHeightAttrPtr);
	I_ATTR(int, m_minimumWindowWidthAttrPtr);
	I_ATTR(int, m_minimumWindowHeightAttrPtr);
	I_ATTR(int, m_visibilityAttrPtr);
};


} // namespace imtqml


