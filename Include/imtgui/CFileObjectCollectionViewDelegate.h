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

	CFileObjectCollectionViewDelegate();

	// reimplemented (ICollectionViewDelegate)
	virtual void UpdateItemSelection(int viewStateFlags, const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId) override;
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const override;
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const override;

protected:
	// reimplemented (CObjectCollectionViewDelegate)
	virtual void SetupCommands() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

protected Q_SLOTS:
	virtual void OnImport();
	virtual void OnExport();

private:
	QByteArray FindTypeIdFromFile(const QString& filePath) const;
	const ifile::IFileTypeInfo* FindFileInfo(const QByteArray& typeId) const;
	QString CreateFileFilter(int flags) const;

private:
	// commands
	iqtgui::CHierarchicalCommand m_importCommand;
	iqtgui::CHierarchicalCommand m_exportCommand;
};


} // namespace imtgui


