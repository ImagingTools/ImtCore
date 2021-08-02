#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <idoc/CSingleDocumentManagerBase.h>
#include <iqtdoc/CSingleDocumentWorkspaceGuiComp.h>

// ImtCore includes
#include <imtgui/IDocumentViewDecorator.h>


namespace imtgui
{


class SdiDocumentViewDecorator;


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
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Size of the icon used in the document command tool bar", true, 16);
		I_ASSIGN(m_showDocumentTitleAttrPtr, "ShowDocumentTitle", "If enabled, the document title is shown inside of the decorator area", true, true);
		I_ASSIGN(m_undoButtonsStyleAttrPtr, "UndoButtonsStyle", "Style of the command buttons for undo manager (See Qt::QToolButtonStyle for reference)", true, Qt::ToolButtonFollowStyle);
		I_ASSIGN(m_fileButtonsStyleAttrPtr, "FileButtonsStyle", "Style of the command buttons for file operations (See Qt::QToolButtonStyle for reference)", true, Qt::ToolButtonFollowStyle);
		I_ASSIGN(m_documentButtonsStyleAttrPtr, "DocumentButtonsStyle", "Style of the command buttons for document operations (See Qt::QToolButtonStyle for reference)", true, Qt::ToolButtonFollowStyle);
	I_END_COMPONENT;

	CSingleDocumentWorkspaceGuiComp();
protected:
	virtual IDocumentViewDecorator* CreateDocumentViewDecorator(
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr);

	// reimplemented (iqtdoc::CSingleDocumentWorkspaceGuiComp)
	virtual void UpdateTitle() override;

	// reimplemented (idoc::CSingleDocumentManagerBase)
	virtual void OnViewRegistered(istd::IPolymorphic* viewPtr) override;
	virtual void OnViewRemoved(istd::IPolymorphic* viewPtr) override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

public Q_SLOTS:
	void OnNew();
	void OnOpen();
	void OnSaveDocument();
	void OnSaveDocumentAs();
	virtual void OnCloseDocument();
	virtual void OnUndo();
	virtual void OnRedo();

protected:
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

protected:
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

	imod::TModelWrap<Commands> m_commands;

	istd::TDelPtr<IDocumentViewDecorator> m_documentViewPtr;

	I_ATTR(int, m_iconSizeAttrPtr);
	I_ATTR(bool, m_showDocumentTitleAttrPtr);
	I_ATTR(int, m_undoButtonsStyleAttrPtr);
	I_ATTR(int, m_fileButtonsStyleAttrPtr);
	I_ATTR(int, m_documentButtonsStyleAttrPtr);
};


} // namespace imtgui


