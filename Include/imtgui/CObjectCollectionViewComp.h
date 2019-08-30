#pragma once


// Qt includes
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QMenu>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtgui/ui_CObjectCollectionViewComp.h>
#include <imtgui/CObjectCollectionViewDelegate.h>

namespace imtgui
{


class CObjectCollectionViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CObjectCollectionViewComp, imtbase::IObjectCollection> BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionViewComp);
		I_ASSIGN(m_viewDelegateCompPtr, "ViewDelegate", "View delegate used for the collection", false, "ViewDelegate");
	I_END_COMPONENT;

	enum DataRole
	{
		DR_TYPE_ID = Qt::UserRole,
		DR_OBJECT_ID
	};

	CObjectCollectionViewComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	ICollectionViewDelegate& GetViewDelegateRef();
	const ICollectionViewDelegate& GetViewDelegate() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiRetranslate() override;
 
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;

private:
	void UpdateCommands();

private Q_SLOTS:
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	QStandardItemModel m_itemModel;
	QVector<QStandardItem*> m_typeItems;

	CObjectCollectionViewDelegate m_defaultViewDelegate;

	I_REF(ICollectionViewDelegate, m_viewDelegateCompPtr);
};


} // namespace imtgui


