#include <imtgui/CFileObjectCollectionViewDelegate.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

// ACF includes
#include <istd/CChangeGroup.h>
#include <ifilegui/CFileDialogLoaderComp.h>

// ImtCore includes
#include <imtbase/IFileObjectCollection.h>


namespace imtgui
{


CFileObjectCollectionViewDelegate::CFileObjectCollectionViewDelegate()
	:m_importCommand("Import", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT),
	m_exportCommand("Export", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_EDIT)
{
}


// reimplemented (ICollectionViewDelegate)

void CFileObjectCollectionViewDelegate::UpdateItemSelection(int viewStateFlags, const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId)
{
	BaseClass::UpdateItemSelection(viewStateFlags, selectedItems, selectedTypeId);

	m_exportCommand.setEnabled(viewStateFlags & VS_SINGLE_SELECTION);
}


QByteArray CFileObjectCollectionViewDelegate::ImportObject(const QByteArray& typeId, const QString& sourcePath) const
{
	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	return fileCollectionPtr->AddFile(sourcePath, typeId);
}


bool CFileObjectCollectionViewDelegate::ExportObject(const QByteArray& objectId, const QString& targetPath) const
{
	if (targetPath.isEmpty()){
		return false;
	}

	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QString resultPath = fileCollectionPtr->GetFile(objectId, targetPath);

	return (resultPath == targetPath);
}


// protected methods

// reimplemented (CFileObjectCollectionViewDelegate)

void CFileObjectCollectionViewDelegate::SetupCommands()
{
	BaseClass::SetupCommands();

	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	if (fileCollectionPtr != nullptr){
		connect(&m_importCommand, SIGNAL(triggered()), this, SLOT(OnImport()));
		connect(&m_exportCommand, SIGNAL(triggered()), this, SLOT(OnExport()));

		m_editCommands.InsertChild(&m_importCommand);
		m_editCommands.InsertChild(&m_exportCommand);
	}
}


// reimplemented (ibase::TLocalizableWrap)

void CFileObjectCollectionViewDelegate::OnLanguageChanged()
{
	BaseClass::OnLanguageChanged();

	m_importCommand.SetVisuals(tr("Import from File..."), tr("Import"), tr("Import existing file into the collection"), QIcon(":/Icons/Load"));
	m_exportCommand.SetVisuals(tr("Export to File..."), tr("Export"), tr("Export data from the collection to a file"), QIcon(":/Icons/Save"));
}


// protected slots

void CFileObjectCollectionViewDelegate::OnImport()
{
	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	istd::CChangeGroup changeGroup(fileCollectionPtr);

	QStringList files = QFileDialog::getOpenFileNames(
				(m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr,
				tr("Import File"),
				QString(),
				CreateFileFilter(ifile::IFilePersistence::QF_LOAD));
	if (!files.isEmpty()){
		for (const QString& filePath : files){
			QByteArray typeId = m_selectedTypeId;
			if (typeId.isEmpty()){
				FindTypeIdFromFile(filePath);
			}

			QByteArray importedId = ImportObject(typeId, filePath);
			if (importedId.isEmpty()){
				QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("Resource could not be imported"));
			}
		}
	}
}


void CFileObjectCollectionViewDelegate::OnExport()
{
	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QByteArray typeId = fileCollectionPtr->GetObjectTypeId(m_selectedItemIds[0]);
	const ifile::IFileTypeInfo* fileInfoPtr = FindFileInfo(typeId);

	QStringList filters;
	QStringList allExt;

	if (fileInfoPtr != nullptr){
		ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*fileInfoPtr, nullptr, -1, allExt, filters, false);
		if (allExt.size() > 1){
			filters.prepend(tr("All known documents (%1)").arg("*." + allExt.join(" *.")));
		}
	}

	istd::CChangeGroup changeGroup(fileCollectionPtr);

	QString filePath = QFileDialog::getSaveFileName(
				(m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr,
				tr("Export File"),
				QString(),
				filters.join(";;"));

	if (!filePath.isEmpty()){
		if (!ExportObject(m_selectedItemIds[0], filePath)){
			QMessageBox::critical((m_parentGuiPtr != nullptr) ? m_parentGuiPtr->GetWidget() : nullptr, tr("Collection"), tr("Resource could not be exported"));
		}
	}
}


// protected methods

QByteArray CFileObjectCollectionViewDelegate::FindTypeIdFromFile(const QString& filePath) const
{
	QFileInfo fileInfo(filePath);
	QString extension = fileInfo.suffix();

	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	const ifile::IFileResourceTypeConstraints* fileConstraintsPtr = fileCollectionPtr->GetResourceTypeConstraints();

	for (int typeIndex = 0; typeIndex < fileConstraintsPtr->GetOptionsCount(); ++typeIndex){
		const ifile::IFileTypeInfo* fileTypeInfoPtr = fileConstraintsPtr->GetFileTypeInfo(typeIndex);
		if (fileTypeInfoPtr != nullptr){
			QStringList extensions;
			fileTypeInfoPtr->GetFileExtensions(extensions);
			if (extensions.contains(extension, Qt::CaseInsensitive)){
				return fileConstraintsPtr->GetOptionId(typeIndex);
			}
		}
	}

	return QByteArray();
}


const ifile::IFileTypeInfo* CFileObjectCollectionViewDelegate::FindFileInfo(const QByteArray& typeId) const
{
	const imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<const imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	const ifile::IFileResourceTypeConstraints* fileConstraintsPtr = fileCollectionPtr->GetResourceTypeConstraints();

	for (int typeIndex = 0; typeIndex < fileConstraintsPtr->GetOptionsCount(); ++typeIndex){
		if (typeId == fileConstraintsPtr->GetOptionId(typeIndex)){
			return fileConstraintsPtr->GetFileTypeInfo(typeIndex);
		}
	}

	return nullptr;
}


QString CFileObjectCollectionViewDelegate::CreateFileFilter(int flags) const
{
	imtbase::IFileObjectCollection* fileCollectionPtr = dynamic_cast<imtbase::IFileObjectCollection*>(m_collectionPtr);
	Q_ASSERT(fileCollectionPtr != nullptr);

	QStringList filters;
	QStringList allExt;

	const ifile::IFileResourceTypeConstraints* fileConstraintsPtr = fileCollectionPtr->GetResourceTypeConstraints();
	for (int typeIndex = 0; typeIndex < fileConstraintsPtr->GetOptionsCount(); ++typeIndex){
		QByteArray typeId = fileConstraintsPtr->GetOptionId(typeIndex);

		if (m_selectedTypeId.isEmpty() || (m_selectedTypeId == typeId)){
			const ifile::IFileTypeInfo* fileTypeInfoPtr = fileConstraintsPtr->GetFileTypeInfo(typeIndex);
			if (fileTypeInfoPtr != nullptr){
				ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*fileTypeInfoPtr, nullptr, -1, allExt, filters, false);
			}
		}
	}

	if ((allExt.size() > 1) && ((flags & ifile::IFilePersistence::QF_SAVE) == 0)){
		filters.prepend(tr("All known documents (%1)").arg("*." + allExt.join(" *.")));
	}

	return filters.join(";;");
}


} // namespace imtgui


