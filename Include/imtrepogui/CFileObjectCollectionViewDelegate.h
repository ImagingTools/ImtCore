#pragma once


// ACF includes
#include <ifile/IFileTypeInfo.h>

// ImtCore includes
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtrepogui
{


/**
	View delegate for a file-based object collection.
	\ingroup Collection
*/
class CFileObjectCollectionViewDelegate: public imtgui::CObjectCollectionViewDelegate
{
	Q_OBJECT
public:
	typedef CObjectCollectionViewDelegate BaseClass;

	enum CommandIdentifier
	{
		CI_IMPORT = BaseClass::CI_USER,
		CI_EXPORT,
		CI_RESTORE,
		CI_USER = BaseClass::CI_USER + 100
	};

	CFileObjectCollectionViewDelegate();

	// reimplemented (ICollectionViewDelegate)
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const override;
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const override;
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;

protected:
	virtual void OnImportObject(const QByteArray& objectId);
	virtual bool IsRestoreAllowed(const QByteArray& objectId);
	virtual void BeforeRestore(const QByteArray& objectId);
	virtual void AfterRestore(const QByteArray& objectId, bool isRestoreSuccessful);

	// reimplemented (imtgui::CObjectCollectionViewDelegate)
	virtual void SetupSummaryInformation() override;
	virtual void SetupCommands() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

protected Q_SLOTS:
	virtual void OnImport();
	virtual void OnExport();
	virtual void OnRestore();

protected:
	enum FileOperationType
	{
		FOT_EXPORT,
		FOT_IMPORT
	};

	virtual QByteArray FindTypeIdFromFile(const QString& filePath) const;
	virtual const ifile::IFileTypeInfo* FindFileInfo(const QByteArray& typeId, FileOperationType operationType) const;
	virtual QString CreateFileImportFilter() const;
	virtual QString CreateFileExportFilter(const QByteArray& objectId) const;

private:
	// commands
	iqtgui::CHierarchicalCommand m_importCommand;
	iqtgui::CHierarchicalCommand m_exportCommand;
	iqtgui::CHierarchicalCommand m_restoreCommand;
};


} // namespace imtrepogui


