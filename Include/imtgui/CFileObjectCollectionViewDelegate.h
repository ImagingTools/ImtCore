#pragma once


// ImtCore includes
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtgui
{


/**
	Standard view delegate for an object collection view.
	\ingroup Collection
*/
class CFileObjectCollectionViewDelegate: public CObjectCollectionViewDelegate
{
	Q_OBJECT
public:
	typedef CObjectCollectionViewDelegate BaseClass;

	CFileObjectCollectionViewDelegate();

	// reimplemented (ICollectionViewDelegate)
	virtual void UpdateCommands(int viewStateFlags, const imtbase::ICollectionInfo::Ids& itemIds) override;
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
	QString CreateFileFilter(int flags) const;

private:
	// commands
	iqtgui::CHierarchicalCommand m_importCommand;
	iqtgui::CHierarchicalCommand m_exportCommand;
};


} // namespace imtgui


