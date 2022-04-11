#include <imtgui/CMultiDocumentWorkspaceGuiComp.h>


// Qt includes
#include <QtGui/QRegularExpressionValidator>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QToolBar>

// ACF includes
#include <imod/IModelEditor.h>
#include <ibase/ICommandsProvider.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iqtgui/CCommandTools.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtgui/IDocumentViewConstraints.h>


namespace imtgui
{


// protected methods

// reimplemented (imtgui::CMultiDocumentWorkspaceGuiCompBase)

IDocumentViewDecorator* CMultiDocumentWorkspaceGuiComp::CreateDocumentViewDecorator(
			istd::IPolymorphic* viewPtr,
			QWidget* parentWidgetPtr,
			const ifile::IFilePersistence* persistencePtr)
{
	MdiDocumentViewDecorator::DecoratorConfiguration configuration;
	configuration.iconSize = *m_iconSizeAttrPtr;
	configuration.fileButtonsStyle = Qt::ToolButtonStyle(*m_fileButtonsStyleAttrPtr);
	configuration.undoButtonsStyle = Qt::ToolButtonStyle(*m_undoButtonsStyleAttrPtr);
	configuration.documentButtonsStyle = Qt::ToolButtonStyle(*m_documentButtonsStyleAttrPtr);

	return new MdiDocumentViewDecorator(this, viewPtr, parentWidgetPtr, persistencePtr, configuration);
}


// reimplemented (idoc::CMultiDocumentManagerBase)

QString CMultiDocumentWorkspaceGuiComp::GetSaveFilePath(const QByteArray& documentTypeId, const istd::IChangeable* dataObjectPtr, const QString& /*currentFilePath*/) const
{
	QInputDialog nameDialog(GetWidget());

	bool isAccepted;
	QString resourceName = nameDialog.getText(GetWidget(), tr("Document Name"), tr("Please enter the name of the document:"), QLineEdit::Normal, QString(), &isAccepted).trimmed();
	if (!isAccepted){
		return QString();
	}

	if (!resourceName.isEmpty()){
		QRegularExpressionValidator inputValidator(QRegularExpression("^[^\\\\/:\\*\\?\"\\<\\>\\|\\+]+$"));
		int pos;
		if (inputValidator.validate(resourceName, pos) != QValidator::Acceptable){
			QMessageBox::critical(NULL, tr("Error"), tr("The document name contains some not allowed characters"));

			return QString();
		}
	}
	else{
		QMessageBox::critical(NULL, tr("Error"), tr("The document name cannot be empty!"));
		return QString();
	}

	const ifile::IFileTypeInfo* fileTypeInfoPtr = GetDocumentFileTypeInfo(documentTypeId);
	if (fileTypeInfoPtr != NULL){
		QStringList extensions;
		if (fileTypeInfoPtr->GetFileExtensions(extensions, dataObjectPtr, ifile::IFileTypeInfo::QF_SAVE)){
			if (!extensions.isEmpty()){
				return resourceName + "." + extensions[0];
			}
		}
	}

	return QString();
}


// reimplemented (idoc::IDocumentManager)

QStringList CMultiDocumentWorkspaceGuiComp::GetOpenFilePaths(const QByteArray* /*documentTypeIdPtr*/) const
{
	QStringList files;

	return files;
}


// reimplemented (iqt:CGuiComponentBase)

void CMultiDocumentWorkspaceGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (!(*m_tabStyleSheetPropertyAttrPtr).isEmpty()){
		Tabs->tabBar()->setProperty(*m_tabStyleSheetPropertyAttrPtr, true);
	}
}


// protected slots

void CMultiDocumentWorkspaceGuiComp::OnSaveDocument()
{
	idoc::IDocumentManager::FileToTypeMap fileMap;

	bool ignoredFlag = false;
	if (SaveDocument(-1, false, &fileMap, false, &ignoredFlag)){
		UpdateAllTitles();
	}
	else if (!ignoredFlag){
		QMessageBox::critical(NULL, "", tr("File could not be saved!"));
	}
}


// protected methods of the embedded class MdiDocumentViewDecorator

MdiDocumentViewDecorator::MdiDocumentViewDecorator(
			CMultiDocumentWorkspaceGuiComp* parentPtr,
			istd::IPolymorphic * viewPtr,
			QWidget * parentWidgetPtr,
			const ifile::IFilePersistence * persistencePtr,
			const DecoratorConfiguration& configuration)
	:BaseClass(parentPtr, viewPtr, parentWidgetPtr, persistencePtr, configuration)
{
}


} // namespace imtgui


