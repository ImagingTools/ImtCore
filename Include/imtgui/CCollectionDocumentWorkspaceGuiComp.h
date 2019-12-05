#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <idoc/CMultiDocumentManagerBase.h>
#include <idoc/IDocumentTemplate.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtdoc/TQtDocumentManagerWrap.h>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QPushButton>


// ImtCore includes
#include <GeneratedFiles/imtgui/ui_CCollectionDocumentWorkspaceGuiComp.h>


namespace imtgui
{


class CCollectionDocumentWorkspaceGuiComp:
			public iqtdoc::TQtDocumentManagerWrap<
						idoc::CMultiDocumentManagerBase,
						iqtgui::TDesignerGuiCompBase<Ui::CCollectionDocumentWorkspaceGuiComp>>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtdoc::TQtDocumentManagerWrap<
						idoc::CMultiDocumentManagerBase,
						iqtgui::TDesignerGuiCompBase<Ui::CCollectionDocumentWorkspaceGuiComp>> BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentWorkspaceGuiComp);
		I_REGISTER_INTERFACE(idoc::IDocumentManager);
		I_REGISTER_INTERFACE(idoc::IDocumentTypesInfo);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_SUBELEMENT(DocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, iprm::ISelectionParam, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, imod::IModel, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, istd::IChangeable, ExtractDocumentList);
		I_REGISTER_SUBELEMENT_INTERFACE(DocumentList, iprm::IOptionsList, ExtractDocumentList);
		I_ASSIGN(m_documentTemplateCompPtr, "DocumentTemplate", "Document template", true, "DocumentTemplate");
		I_ASSIGN(m_showMaximizedAttrPtr, "ShowViewMaximized", "At start shows the document view maximized", false, true);
		I_ASSIGN(m_showPathAsTipAttrPtr, "ShowFilePathAsToolTip", "if enabled, the current document file path will be shown as tool tip of document tab", true, false);
		I_ASSIGN(m_allowViewRepeatingAttrPtr, "AllowViewRepeating", "If enabled, multiple views for the same document are allowed", false, true);
		I_ASSIGN(m_rememberOpenDocumentsParamPtr, "RememberOpenDocumentsOnExit", "If enabled, restores open documents from previous session", false, "RememberOpenDocumentsOnExit");
		I_ASSIGN(m_workspaceBackgroundColorAttrPtr, "WorkspaceBackgroundColor", "Background color of the MDI workspace", false, "");
		I_ASSIGN(m_defaultCreatedDocumentTypeIdAttrPtr, "DefaultCreatedDocumentTypeId", "Type-ID of the document that should be created if the workspace is empty", false, "");
	I_END_COMPONENT;

	enum GroupId
	{
		GI_WINDOW = 0x300,
		GI_DOCUMENT,
		GI_VIEW
	};

	CCollectionDocumentWorkspaceGuiComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const;

	// reimplemented (iqtgui::IGuiObject)
	virtual void OnTryClose(bool* ignoredPtr = NULL);

protected:

/**
		Update titles of views or all views of specified document.
		\param	optional document object, if you want to update only views of single document.
	*/
	void UpdateAllTitles();

	/**
		Find view object associated with specified Qt widget.
		\param	widget	Qt widget representing view.
		\return	associated view or NULL if nothing is found.
	*/
	iqtgui::IGuiObject* GetViewFromWidget(const QWidget& widget) const;

	/**
		Find index of document associated with specified Qt widget.
		\param	widget	Qt widget representing view.
		\return	index of document or negative value if nothing is found.
	*/
	int GetDocumentIndexFromWidget(const QWidget& widget) const;

	/**
		Called when number of windows changed.
	*/
	void OnViewsCountChanged();

	// reimplemented (idoc::CMultiDocumentManagerBase)
	virtual istd::IChangeable* OpenSingleDocument(
				const QString& filePath,
				bool createView,
				const QByteArray& viewTypeId,
				QByteArray& documentTypeId,
				bool beQuiet,
				bool* ignoredPtr,
				ibase::IProgressManager* progressManagerPtr);
	virtual void SetActiveView(istd::IPolymorphic* viewPtr);

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* sourcePtr, QEvent* eventPtr);

	// reimplemented (idoc::CMultiDocumentManagerBase)
	virtual void CloseAllDocuments();
	virtual QStringList GetOpenFilePaths(const QByteArray* documentTypeIdPtr = NULL) const;
	virtual void OnViewRegistered(istd::IPolymorphic* viewPtr, const SingleDocumentData& documentData);
	virtual void OnViewRemoved(istd::IPolymorphic* viewPtr);
	virtual bool QueryDocumentSave(const SingleDocumentData& info, bool* ignoredPtr);

	// reimplemented (iqt:CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();
	virtual void OnRetranslate();
	virtual void OnGuiRetranslate();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

	// reimplemented (istd:IChangeable)
	virtual void OnEndChanges(const ChangeSet& changeSet);

protected Q_SLOTS:
	void OnCloseAllViews();

private slots:
	void OnBtnBack();
	void OnBtnSave();
	void OnBtnClose();

private:
	class DocumentList: virtual public iprm::ISelectionParam, virtual public iprm::IOptionsList
	{
	public:
		DocumentList();

		void SetParent(CCollectionDocumentWorkspaceGuiComp& parent);

		// reimplemented (iprm::ISelectionParam)
		virtual const iprm::IOptionsList* GetSelectionConstraints() const;
		virtual int GetSelectedOptionIndex() const;
		virtual bool SetSelectedOptionIndex(int index);
		virtual iprm::ISelectionParam* GetSubselection(int index) const;

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const;
		virtual int GetOptionsCount() const;
		virtual QString GetOptionName(int index) const;
		virtual QString GetOptionDescription(int index) const;
		virtual QByteArray GetOptionId(int index) const;
		virtual bool IsOptionEnabled(int index) const;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive);

	private:
		int m_selectedDocumentIndex;
		CCollectionDocumentWorkspaceGuiComp* m_parent;
	};

	friend class imod::TModelWrap<DocumentList>;

	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractDocumentList(CCollectionDocumentWorkspaceGuiComp& component)
	{
		return &component.m_documentList;
	}

	iqtgui::CHierarchicalCommand m_commands;

	// global commands
	iqtgui::CHierarchicalCommand m_windowCommand;

	// window menu group
	iqtgui::CHierarchicalCommand m_closeAllDocumentsCommand;

	mutable QString m_lastDirectory;

	int m_viewsCount;

	imod::TModelWrap<DocumentList> m_documentList;

	QString m_organizationName;
	QString m_applicationName;

	bool m_forceQuietClose;

	I_REF(idoc::IDocumentTemplate, m_documentTemplateCompPtr);
	I_REF(iprm::IEnableableParam, m_rememberOpenDocumentsParamPtr);
	I_ATTR(bool, m_showMaximizedAttrPtr);
	I_ATTR(bool, m_showPathAsTipAttrPtr);
	I_ATTR(bool, m_allowViewRepeatingAttrPtr);
	I_ATTR(QString, m_workspaceBackgroundColorAttrPtr);
	I_ATTR(QByteArray, m_defaultCreatedDocumentTypeIdAttrPtr);

	QToolBar *m_pToolBar;
	QPushButton *m_pBtnBack;
	QPushButton *m_pBtnSave;
	QPushButton *m_pBtnClose;
};


} // namespace imtgui


