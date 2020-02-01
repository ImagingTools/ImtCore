#pragma once


// Qt includes
#include <QtCore/QtGlobal>

#if QT_VERSION >= 0x050400
#include <QtWebEngineWidgets/QWebEngineView>
#define QWebView QWebEngineView
#else
#include <QtWebkitWidgets/QWebView>
#endif

// ACF includes
#include <iqtgui/TGuiComponentBase.h>
#include <idoc/IHelpViewer.h>
#include <idoc/IHelpFileProvider.h>


namespace qhelpgui
{


class QHtmlHelpGuiComp:
			public iqtgui::TGuiComponentBase<QWebView>,
			virtual public idoc::IHelpViewer
{
public:
	typedef iqtgui::TGuiComponentBase<QWebView> BaseClass;

	I_BEGIN_COMPONENT(QHtmlHelpGuiComp);
		I_REGISTER_INTERFACE(idoc::IHelpInfoProvider);
		I_REGISTER_INTERFACE(idoc::IHelpViewer);
		I_ASSIGN(m_helpFileProviderCompPtr, "HelpFileProvider", "Calculate path of html document", true, "HelpFileProvider");
	I_END_COMPONENT;

	// reimplemented (idoc::IHelpInfoProvider)
	virtual double GetHelpQuality(const QString& contextText, const istd::IPolymorphic* contextObjectPtr) const;

	// reimplemented (idoc::IHelpViewer)
	virtual void ShowHelp(const QString& contextText, const istd::IPolymorphic* contextObjectPtr);

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiShown();

private:
	I_REF(idoc::IHelpFileProvider, m_helpFileProviderCompPtr);

	mutable QUrl m_urlToShow;
};


} // namespace qhelpgui
