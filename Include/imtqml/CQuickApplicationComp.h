// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QEventLoop>
#include <QtQml/QQmlIncubator>
#include <QtQuick/QQuickWindow>

// ImtCore includes
#include <imtqml/CQuickApplicationCompBase.h>
#include <imtqml/IQuickObject.h>


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
		I_ASSIGN(m_splashCompPtr, "SplashScreen", "Splash QML object (root must be an Item) shown in a fixed-size window while the main UI is loading. If not set or not loadable, the built-in splash is used", false, "SplashScreen");
	I_END_COMPONENT;

	CQuickApplicationComp();

	// reimplemented (ibase::IGuiApplication)
	virtual const imtqml::IQuickObject* GetApplicationItem() const;

	// reimplemented (ibase::IApplication)
	virtual bool InitializeApplication(int argc, char** argv) override;
	virtual int Execute(int argc, char** argv) override;
	virtual QString GetHelpText() const override;

protected:
	class CLoopIncubator: public QQmlIncubator
	{
	public:
		CLoopIncubator(QEventLoop& loop) : QQmlIncubator(QQmlIncubator::Asynchronous), m_loop(loop)
		{}

	protected:
		void statusChanged(Status status) override
		{
			if (status == Ready || status == Error){
				QQuickWindow* windowPtr = qobject_cast<QQuickWindow*>(object());
				if (windowPtr != nullptr){
					windowPtr->setVisible(false);
				}
				m_loop.quit();
			}
		}

	private:
		QEventLoop& m_loop;
	};

	static const QByteArray s_splashQml;
	static const QByteArray s_splashHostQml;
	static const QByteArray s_settingsGroup;

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
	I_REF(imtqml::IQuickObject, m_splashCompPtr);
};


} // namespace imtqml


