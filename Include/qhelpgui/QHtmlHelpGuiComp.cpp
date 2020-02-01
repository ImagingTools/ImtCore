#include <qhelpgui/QHtmlHelpGuiComp.h>


namespace qhelpgui
{


// reimplemented (idoc::IHelpInfoProvider)

double QHtmlHelpGuiComp::GetHelpQuality(const QString& contextText, const istd::IPolymorphic* contextObjectPtr) const
{
	if (m_helpFileProviderCompPtr.IsValid()){
		return m_helpFileProviderCompPtr->GetHelpQuality(contextText, contextObjectPtr);
	}

	return 0;
}


// reimplemented (idoc::IHelpViewer)

void QHtmlHelpGuiComp::ShowHelp(const QString& contextText, const istd::IPolymorphic* contextObjectPtr)
{
	QWebView* editorPtr = GetQtWidget();
	if (		(editorPtr != NULL) &&
				m_helpFileProviderCompPtr.IsValid() &&
				(m_helpFileProviderCompPtr->GetHelpQuality(contextText, contextObjectPtr) > 0)){
		QString filePath = m_helpFileProviderCompPtr->GetHelpFilePath(contextText, contextObjectPtr);

		QUrl url = QUrl::fromLocalFile(filePath);

		if (editorPtr->isVisible()){
			editorPtr->load(url);

			m_urlToShow.clear();
		}
		else{
			m_urlToShow = url;
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void QHtmlHelpGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	ShowHelp("", NULL);
}


void QHtmlHelpGuiComp::OnGuiShown()
{
	BaseClass::OnGuiShown();

	QWebView* editorPtr = GetQtWidget();
	if ((editorPtr != NULL) && !m_urlToShow.isEmpty()){
		editorPtr->load(m_urlToShow);
	}
}


} // namespace qhelpgui


