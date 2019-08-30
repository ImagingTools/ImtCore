#pragma once


// Qt includes
#include <QtWidgets/QMenu>

// ACF includes
#include <ibase/TLocalizableWrap.h>
#include <iqtgui/IGuiObject.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtgui/ICollectionViewDelegate.h>


namespace imtgui
{


/**
	Standard view delegate for an object collection view.
	\ingroup Collection
*/
class CObjectCollectionViewDelegate: public QObject, public ibase::TLocalizableWrap<ICollectionViewDelegate>
{
	Q_OBJECT
public:
	CObjectCollectionViewDelegate();

	// reimplemented (ICollectionViewDelegate)
	virtual bool InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr) override;
	virtual QByteArrayList GetObjectTypeIds() const override;
	virtual void UpdateCommands(int viewStateFlags, const imtbase::ICollectionInfo::Ids& itemIds) override;
	virtual QByteArray CreateNewObject(const QByteArray& typeId) const override;
	virtual QByteArray ImportObject(const QByteArray& typeId, const QString& sourcePath = QString()) const override;
	virtual bool ExportObject(const QByteArray& objectId, const QString& targetPath = QString()) const override;
	virtual void RemoveObjects(const imtbase::ICollectionInfo::Ids& objectIds)  const override;
	virtual bool UpdateObject(const QByteArray& objectId, const istd::IChangeable& contents) const override;
	virtual bool RenameObject(const QByteArray& objectId, const QString& newName) const override;

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

private Q_SLOTS:
	void OnInsert();
	void OnRemove();
	void OnAddMenuOptionClicked(QAction* action);

private:
	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_editCommands;
	iqtgui::CHierarchicalCommand m_insertCommand;
	iqtgui::CHierarchicalCommand m_removeCommand;

	QMenu m_startVariableMenus;

	imtbase::IObjectCollection* m_collectionPtr;
	iqtgui::IGuiObject* m_parentGuiPtr;

	imtbase::ICollectionInfo::Ids m_selectedItemIds;
};


} // namespace imtgui


