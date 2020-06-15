#pragma once


// ACF includes
#include <ifile/IFileTypeInfo.h>

// ImtCore includes
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtgui
{


/**
	View delegate for a file-based object collection.
	\ingroup Collection
*/
class CFileObjectCollectionViewDelegate: public CObjectCollectionViewDelegate
{
	Q_OBJECT
public:
	typedef CObjectCollectionViewDelegate BaseClass;

	enum CommandIdentifier
	{
		CI_IMPORT = BaseClass::CI_USER,
		CI_EXPORT,
		CI_USER = BaseClass::CI_USER + 100
	};

	CFileObjectCollectionViewDelegate();

	// reimplemented (ICollectionViewDelegate)
	virtual void UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const override;
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const override;
	virtual SummaryInformation GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const override;
	virtual bool IsCommandSupported(int commandId) const override;

protected:
	// reimplemented (CObjectCollectionViewDelegate)
	virtual void SetupCommands() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

protected Q_SLOTS:
	virtual void OnImport();
	virtual void OnExport();

protected:
	QByteArray m_lastImportedObjectId;

protected:
	QByteArray FindTypeIdFromFile(const QString& filePath) const;
	const ifile::IFileTypeInfo* FindFileInfo(const QByteArray& typeId) const;
	QString CreateFileFilter(int flags) const;

private:
	// commands
	iqtgui::CHierarchicalCommand m_importCommand;
	iqtgui::CHierarchicalCommand m_exportCommand;
};


} // namespace imtgui


