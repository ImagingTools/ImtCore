#pragma once


// ACF includes
#include <idoc/CSingleDocumentManagerBase.h>
#include <iqtdoc/CSingleDocumentWorkspaceGuiComp.h>

// ImtCore includes
#include <imtgui/TStandardDocumentViewDecorator.h>
#include <imtgui/TStandardDocumentViewDecorator.h>
#include <GeneratedFiles/imtgui/ui_CStandardDocumentViewDecorator.h>


namespace imtgui
{


class CSingleDocumentWorkspaceGuiComp: public iqtdoc::CSingleDocumentWorkspaceGuiComp, public imod::CMultiModelDispatcherBase
{
	Q_OBJECT

public:
	typedef iqtdoc::CSingleDocumentWorkspaceGuiComp BaseClass;

	I_BEGIN_COMPONENT(CSingleDocumentWorkspaceGuiComp);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
	I_END_COMPONENT;

	CSingleDocumentWorkspaceGuiComp();
protected:
	virtual IDocumentViewDecorator* CreateDocumentViewDecorator(
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr);

	// reimplemented (idoc::CSingleDocumentManagerBase)
	virtual void OnViewRegistered(istd::IPolymorphic* viewPtr) override;
	virtual void OnViewRemoved(istd::IPolymorphic* viewPtr) override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

protected Q_SLOTS:
	void OnNew();
	void OnOpen();
	void OnSaveDocument();
	void OnSaveDocumentAs();
	virtual void OnCloseDocument();
	virtual void OnUndo();
	virtual void OnRedo();

private:
	class Commands: virtual public ibase::ICommandsProvider
	{
	public:
		Commands();

		void SetParent(CSingleDocumentWorkspaceGuiComp* parentPtr);

	protected:
		// reimplemented (ibase::ICommandsProvider)
		virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	private:
		CSingleDocumentWorkspaceGuiComp* m_parentPtr;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractCommands(CSingleDocumentWorkspaceGuiComp& component)
	{
		return &component.m_commands;
	}

private:
	enum DataRole
	{
		DR_PATH = Qt::UserRole,
		DR_TYPE_ID
	};
	enum ModelIds
	{
		MI_VISUAL_STATUS,
		MI_DOCUMENT_COMMANDS
	};

	friend class SdiDocumentViewDecorator;
	friend class TStandardDocumentViewDecorator<CSingleDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator>;

	imod::TModelWrap<Commands> m_commands;
};


class SdiDocumentViewDecorator: public TStandardDocumentViewDecorator<CSingleDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator>
{
public:
	typedef TStandardDocumentViewDecorator<CSingleDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator> BaseClass;

	SdiDocumentViewDecorator(
				CSingleDocumentWorkspaceGuiComp* parentPtr,
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr);
};


} // namespace imtgui


