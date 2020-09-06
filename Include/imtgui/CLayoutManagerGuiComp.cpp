#include <imtgui/CLayoutManagerGuiComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <QtGui/QIcon>

// ACF includes
#include <istd/CChangeGroup.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>


namespace imtgui
{


// public methods

CLayoutManagerGuiComp::CLayoutManagerGuiComp()
	:m_layoutWidgetPtr(nullptr),
	m_commands("&View", 100),
	m_startEndEditModeCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, 1988),
	m_clearCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, 1988),
	m_loadCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, 1988),
	m_saveCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, 1988)
{
	m_rootCommands.InsertChild(&m_commands);
	m_commands.InsertChild(&m_startEndEditModeCommand);
	m_commands.InsertChild(&m_clearCommand);
	m_commands.InsertChild(&m_loadCommand);
	m_commands.InsertChild(&m_saveCommand);
	m_clearCommand.setVisible(false);
	m_clearCommand.setCheckable(false);
	m_loadCommand.setVisible(false);
	m_loadCommand.setCheckable(false);
	m_saveCommand.setVisible(false);
	m_saveCommand.setCheckable(false);

	
	connect(&m_startEndEditModeCommand, SIGNAL(triggered()), this, SLOT(OnStartEndEditCommand()));
	connect(&m_clearCommand, SIGNAL(triggered()), this, SLOT(OnClearAll()));
	connect(&m_loadCommand, SIGNAL(triggered()), this, SLOT(OnLoad()));
	connect(&m_saveCommand, SIGNAL(triggered()), this, SLOT(OnSave()));

	m_splitterTimer.setSingleShot(true);
	connect(&m_splitterTimer, SIGNAL(timeout()), this, SLOT(OnSplitterMoveFinished()));
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CLayoutManagerGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

QLayout* CLayoutManagerGuiComp::CreateCustomLayoutWidget(ILayout* layout)
{
	QLayout* retVal = new QVBoxLayout();
	if (layout->GetType() == ILayout::LT_NONE){
		CCustomLayoutWidget* customLayoutWidgetPtr = m_layoutWidgetPtr->createCustomWidget();
		customLayoutWidgetPtr->SetId(layout->GetLayoutId());
		customLayoutWidgetPtr->SetIcon(layout->GetIcon());
		customLayoutWidgetPtr->SetTitleAlign(layout->GetTitleAlign());
		customLayoutWidgetPtr->SetName(layout->GetTitle());
		customLayoutWidgetPtr->SetLayoutProperties(layout->GetLayoutProperties());

		QByteArray viewId = layout->GetViewId();
		if (!viewId.isEmpty()){
			int index = m_guiViewIdMultiAttrPtr.FindValue(viewId);
			if (index >= 0){
				istd::TSmartPtr<iqtgui::IGuiObject> guiPtr(m_guiViewMultiFactCompPtr.CreateInstance(index));
				if (guiPtr->CreateGui(nullptr)){
					customLayoutWidgetPtr->SetWidget(guiPtr->GetWidget());
					customLayoutWidgetPtr->SetViewId(viewId);

					m_guiObjects.push_back(guiPtr);
				}
			}
		}

		retVal->addWidget(customLayoutWidgetPtr);

		retVal->setContentsMargins(
					customLayoutWidgetPtr->LeftMargin(),
					customLayoutWidgetPtr->TopMargin(),
					customLayoutWidgetPtr->RightMargin(),
					customLayoutWidgetPtr->BottomMargin());
	}
	else{
		ILayout::LayoutProperties properties = layout->GetLayoutProperties();
		
		QSplitter* splitterPtr = new QSplitter();
		connect(splitterPtr, SIGNAL(splitterMoved(int, int)), this, SLOT(OnSplitterMoved(int, int)));
		SplittersMap.insert(splitterPtr, layout->GetLayoutId());

		retVal->addWidget(splitterPtr);
		retVal->setMargin(0);

		if (layout->GetType() == ILayout::LT_HORIZONTAL_SPLITTER){
			splitterPtr->setOrientation(Qt::Horizontal);
		}
		else{
			splitterPtr->setOrientation(Qt::Vertical);
		}
			
		for (int i = 0; i < layout->GetChildsCount(); i++){
			ILayout* childLayoutPtr = layout->GetChild(i);
			if (childLayoutPtr != nullptr){
				QWidget* widget = new QWidget();
				widget->setLayout(CreateCustomLayoutWidget(childLayoutPtr));
				splitterPtr->addWidget(widget);
				if ( properties.isFixedLayout && (m_layoutWidgetPtr->GetViewMode() == CHierarchicalLayoutWidget::VM_NORMAL)){
					QSplitterHandle* splitterHandlerPtr = splitterPtr->handle(i);
					if (splitterHandlerPtr != nullptr){
						splitterHandlerPtr->setEnabled(false);
					}

					splitterPtr->setHandleWidth(0);
				}
			}
		}
		SizeList sizes = layout->GetSizes();
		if (sizes.count() >= splitterPtr->count()){
			splitterPtr->setSizes(layout->GetSizes());
		}
	}

	return retVal;
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CLayoutManagerGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_layoutWidgetPtr != nullptr){
		SplittersMap.clear();

		RemoveAllGuiComponents();

		m_layoutWidgetPtr->ClearAll();

		m_layoutWidgetPtr->setLayout(CreateCustomLayoutWidget(GetObservedObject()));
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CLayoutManagerGuiComp::OnGuiCreated()
{
	m_layoutWidgetPtr = new CHierarchicalLayoutWidget(GetQtWidget());
	this->GetWidget()->layout()->addWidget(m_layoutWidgetPtr);

	// only DirectConnection for valid DropEvent processing
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitDropEvent(QByteArray, QDropEvent*)), this, SLOT(OnDropWidget(QByteArray, QDropEvent*)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitOpenMenuEvent(QByteArray, QMouseEvent*)), this, SLOT(OnOpenMenu(QByteArray, QMouseEvent*)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitClearEvent(QByteArray)), this, SLOT(OnClearWidget(QByteArray)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitAddWidget(const QByteArray&,int)), this, SLOT(OnAddWidget(const QByteArray&, int)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitAddWidgetByViewId(const QByteArray&, const QByteArray&)), this, SLOT(OnAddWidgetByViewId(const QByteArray&, const QByteArray&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitDeleteWidget(const QByteArray&)), this, SLOT(OnDeleteWidget(const QByteArray&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitSplitLayout(const QByteArray&, ILayout::LayoutType, int, int)), this, SLOT(OnSplitLayout(const QByteArray&, ILayout::LayoutType, int, int)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeIcon(const QByteArray&)), this, SLOT(OnChangeIcon(const QByteArray&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitRemoveIcon(const QByteArray&)), this, SLOT(OnRemoveIcon(const QByteArray&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeTitle(const QByteArray&, const QString&)), this, SLOT(OnChangeTitle(const QByteArray&, const QString&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeAlignTitle(const QByteArray&, const ILayout::AlignType&)), this, SLOT(OnChangeAlignTitle(const QByteArray&, const ILayout::AlignType&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeSizes(const QByteArray&, const SizeList&)), this, SLOT(OnChangeSizes(const QByteArray&, const SizeList&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeProperties(const QByteArray&)), this, SLOT(OnChangeProperties(const QByteArray&)), Qt::DirectConnection);
	
	// check views attributes
	Q_ASSERT_X(m_guiViewIdMultiAttrPtr.IsValid(), "CLayoutManagerGuiComp", "attribute ViewIds should be set");
	Q_ASSERT_X(m_guiViewMultiFactCompPtr.IsValid(), "CLayoutManagerGuiComp", "attribute ViewFactories should be set");
	Q_ASSERT_X(m_guiViewNameMultiAttrPtr.IsValid(), "CLayoutManagerGuiComp", "attribute ViewNames should be set");
	Q_ASSERT_X((m_guiViewIdMultiAttrPtr.GetCount() == m_guiViewMultiFactCompPtr.GetCount()) &&
		(m_guiViewNameMultiAttrPtr.GetCount() == m_guiViewMultiFactCompPtr.GetCount()), "CLayoutManagerGuiComp", "attributes ViewIds, ViewNames and ViewFactories should have the same count");
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);
	ILayout::LayoutProperties properties;
	if (m_colorBorderAttrPtr.IsValid()){
		properties = rootLayoutPtr->GetLayoutProperties();
		properties.borderColor = QColor(QString(*m_colorBorderAttrPtr));
		rootLayoutPtr->SetLayoutProperties(properties);
	}
	if (m_isBorderEnabledAttrPtr.IsValid()){
		properties = rootLayoutPtr->GetLayoutProperties();
		properties.isBorderEnabled = *m_isBorderEnabledAttrPtr;
		rootLayoutPtr->SetLayoutProperties(properties);
	}
	if (m_isFixedLayoutAttrPtr.IsValid()){
		properties = rootLayoutPtr->GetLayoutProperties();
		properties.isFixedLayout = *m_isFixedLayoutAttrPtr;
		rootLayoutPtr->SetLayoutProperties(properties);
	}
	if (m_isEditEnabledAttrPtr.IsValid()){
		m_startEndEditModeCommand.setVisible(*m_isEditEnabledAttrPtr);
	}

	// add undo manager commands provider
	if (m_commandsProviderCompPtr.IsValid()){
		const iqtgui::CHierarchicalCommand *commands = dynamic_cast<const iqtgui::CHierarchicalCommand*>(m_commandsProviderCompPtr->GetCommands());
		for (int i = 0; i < commands->GetChildsCount(); i++){
			iqtgui::CHierarchicalCommand *command = dynamic_cast<iqtgui::CHierarchicalCommand*>(commands->GetChild(i));
			if (command != nullptr){
				m_undoCommands = command;
				m_rootCommands.InsertChild(m_undoCommands);
				for (int j = 0; j < m_undoCommands->GetChildsCount(); j++){
					iqtgui::CHierarchicalCommand *commandChild = dynamic_cast<iqtgui::CHierarchicalCommand*>(m_undoCommands->GetChild(j));
					if (commandChild != nullptr){
						commandChild->setVisible(false);
					}
				}
				break;
			}
		}
	}

	// gui views part
	QStringList additionalNames;
	if (m_guiViewIdMultiAttrPtr.IsValid() && m_guiViewMultiFactCompPtr.IsValid() && m_guiViewNameMultiAttrPtr.IsValid()){
		int minCount = qMin(m_guiViewIdMultiAttrPtr.GetCount(), m_guiViewMultiFactCompPtr.GetCount());
		minCount = qMin(minCount, m_guiViewNameMultiAttrPtr.GetCount());
		for (int i = 0; i < minCount; ++i){
			additionalNames.append(m_guiViewNameMultiAttrPtr[i]);
			m_guiViewOptionsManager.InsertOption(m_guiViewNameMultiAttrPtr[i], m_guiViewIdMultiAttrPtr[i]);
		}
	}

	m_layoutWidgetPtr->SetAdditionalNames(additionalNames);

	BaseClass::OnGuiCreated();
}


void CLayoutManagerGuiComp::OnGuiDestroyed()
{
	RemoveAllGuiComponents();

	BaseClass::OnGuiDestroyed();
}


void CLayoutManagerGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	// File commands emptyIcon
	m_startEndEditModeCommand.SetVisuals(tr("Edit Mode"), tr("Edit Mode"), tr("EditMode"), QIcon(":/Icons/Edit"));
	m_clearCommand.SetVisuals(tr("Clear All"), tr("Clear All"), tr("ClearAll"), QIcon(":/Icons/Clear"));
	m_loadCommand.SetVisuals(tr("Import"), tr("Import"), tr("Import"), QIcon(":/Icons/Import"));
	m_saveCommand.SetVisuals(tr("Export"), tr("Export"), tr("Export"), QIcon(":/Icons/Export"));
}


// protected slots

void CLayoutManagerGuiComp::OnSplitterMoved(int /*pos*/, int /*index*/)
{
	QSplitter* splitterPtr = dynamic_cast<QSplitter*>(sender());
	if (splitterPtr != NULL && SplittersMap.contains(splitterPtr)){
		m_activeId = SplittersMap.value(splitterPtr);
		m_splitterTimer.stop();
		m_splitterTimer.start(1000);
	}
}


void CLayoutManagerGuiComp::OnSplitterMoveFinished()
{
	QMap<QSplitter*, QByteArray>::const_iterator i = SplittersMap.constBegin();
	while (i != SplittersMap.constEnd()){
		if (i.value() == m_activeId){
			QSplitter* splitterPtr = i.key();
			OnChangeSizes(m_activeId, splitterPtr->sizes());
			break;
		}
		++i;
	}
}


void CLayoutManagerGuiComp::OnStartEndEditCommand()
{
	if (m_layoutWidgetPtr != nullptr){
		QAction* actionPtr = dynamic_cast<QAction*>(sender());
		if (actionPtr != nullptr){
			if (actionPtr->isChecked()){
				m_layoutWidgetPtr->SetViewMode(CHierarchicalLayoutWidget::VM_EDIT);
				m_clearCommand.setVisible(true);
				m_loadCommand.setVisible(true);
				m_saveCommand.setVisible(true);
				for (int i = 0; i < m_undoCommands->GetChildsCount(); i++){
					dynamic_cast<iqtgui::CHierarchicalCommand*>(m_undoCommands->GetChild(i))->setVisible(true);
				}

			}
			else{
				m_layoutWidgetPtr->SetViewMode(CHierarchicalLayoutWidget::VM_NORMAL);
				m_clearCommand.setVisible(false);
				m_loadCommand.setVisible(false);
				m_saveCommand.setVisible(false);
				for (int i = 0; i < m_undoCommands->GetChildsCount(); i++){
					dynamic_cast<iqtgui::CHierarchicalCommand*>(m_undoCommands->GetChild(i))->setVisible(false);
				}

			}

			const istd::IChangeable::ChangeSet changeSet;
			UpdateGui(changeSet);

			//QMap<QSplitter*, QByteArray>::const_iterator iter = SplittersMap.constBegin();
			//while (iter != SplittersMap.constEnd()){
			//	QSplitter* splitterPtr = iter.key();
			//	for (int i = 0; i < splitterPtr->count(); i++){
			//		QSplitterHandle *hndl = splitterPtr->handle(i);
			//		bool fixedSplitter = false;
			//		if (actionPtr->isChecked() == false && m_isFixedLayoutPtr.IsValid() == true
			//			&& *m_isFixedLayoutPtr == true){
			//			fixedSplitter = true;
			//		}
			//		hndl->setEnabled(!fixedSplitter);
			//	}
			//	++iter;
			//}
		}
	}
}


void CLayoutManagerGuiComp::OnClearAll()
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	if (m_layoutWidgetPtr != nullptr){
		QMessageBox msgBox;
		msgBox.setText(tr("Do you want clear all layout items?"));
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);

		int ret = msgBox.exec();
		if (ret == QMessageBox::Ok){
			rootLayoutPtr->Clear();
		}
	}

	RemoveAllGuiComponents();
}


void CLayoutManagerGuiComp::OnLoad()
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	QString fileName = QFileDialog::getOpenFileName(GetWidget(), tr("Import Layout"), QString(), QString("Layout Files (*.layout)"));
	if (!fileName.isEmpty()){
		ifile::CCompactXmlFileReadArchive archive(fileName);

		bool retVal = rootLayoutPtr->Serialize(archive);
		if (!retVal){
			QMessageBox::critical(GetWidget(), tr("Import Layout..."), tr("Layout could not be imported"));
		}
	}
}


void CLayoutManagerGuiComp::OnSave()
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	QString fileName = QFileDialog::getSaveFileName(GetWidget(), tr("Export Layout"), QString(), QString("Layout Files (*.layout)"));
	if (!fileName.isEmpty()){
		ifile::CCompactXmlFileWriteArchive archive(fileName);

		bool retVal = rootLayoutPtr->Serialize(archive);
		if (!retVal){
			QMessageBox::critical(GetWidget(), tr("Export Layout..."), tr("Layout could not be exported"));
		}
	}
}


void CLayoutManagerGuiComp::OnChangeTitle(const QByteArray& id, const QString& title)
{
	// The title was already changed in the editor, therefore we don't need any UI-updates anymore,
	// just set the new title value to the data model:
	if (!IsUpdateBlocked()){
		UpdateBlocker blockUpdates(this);

		ILayout* rootLayoutPtr = GetObservedObject();
		Q_ASSERT(rootLayoutPtr != nullptr);

		if (rootLayoutPtr != nullptr){
			ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
			if (childLayoutPtr != nullptr){
				childLayoutPtr->SetTitle(title);
			}
		}
	}
}


void CLayoutManagerGuiComp::OnChangeAlignTitle(const QByteArray& id, const ILayout::AlignType& align)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		childLayoutPtr->SetTitleAlign(align);
	}
}


void CLayoutManagerGuiComp::OnChangeIcon(const QByteArray& id)
{
	QString fileName = QFileDialog::getOpenFileName(
				nullptr,
				tr("Open Image"),
				"",
				tr("Image Files (*.png *.jpg *.bmp *.ico *.svg)"));

	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	if (!fileName.isEmpty()){
		ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
		if (childLayoutPtr != nullptr){
			QPixmap pixmap(fileName);
			pixmap = pixmap.scaled(28, 28);

			childLayoutPtr->SetIcon(pixmap);
		}
	}
}


void CLayoutManagerGuiComp::OnRemoveIcon(const QByteArray& id)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

		ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
		if (childLayoutPtr != nullptr) {
			childLayoutPtr->SetIcon(QPixmap());
		}
}


void CLayoutManagerGuiComp::OnSplitLayout(const QByteArray& id, ILayout::LayoutType type, int width, int height)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	istd::CChangeGroup changeGroup(rootLayoutPtr);

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		childLayoutPtr->SplitLayout(type, width, height);
	}
}


void CLayoutManagerGuiComp::OnDeleteWidget(const QByteArray& id)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	delete rootLayoutPtr->RemoveChild(id);
}


void CLayoutManagerGuiComp::OnAddWidget(const QByteArray& id, int index)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	QByteArray viewId;

	if (index >= 0){
		viewId = m_guiViewIdMultiAttrPtr[index];
	}

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		childLayoutPtr->SetViewId(viewId);
	}

}


void CLayoutManagerGuiComp::OnAddWidgetByViewId(const QByteArray& id, const QByteArray& viewId)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		childLayoutPtr->SetViewId(viewId);
	}

}


void CLayoutManagerGuiComp::OnChangeSizes(const QByteArray& id, const SizeList& sizeList)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		childLayoutPtr->SetSizes(sizeList);
	}
}


void CLayoutManagerGuiComp::OnChangeProperties(const QByteArray& id)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	ILayout::LayoutProperties dialogProperties;

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		dialogProperties = childLayoutPtr->GetLayoutProperties();

		CLayoutSettingsDialog settingsDialog;
		settingsDialog.SetLayoutProperties(dialogProperties);
		if (settingsDialog.exec() == QDialog::Accepted){
			dialogProperties = settingsDialog.GetLayoutProperties();

			istd::CChangeGroup changeGroup(rootLayoutPtr);

			ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
			if (childLayoutPtr != nullptr){
				childLayoutPtr->SetLayoutProperties(dialogProperties);
				ILayout* parentLayoutPtr = childLayoutPtr->GetParent();
				if (parentLayoutPtr != nullptr){
					bool isFixedLayout = dialogProperties.isFixedLayout;
					ILayout::LayoutProperties layoutProperties = parentLayoutPtr->GetLayoutProperties();
					layoutProperties.isFixedLayout = isFixedLayout;
					parentLayoutPtr->SetLayoutProperties(layoutProperties);
					for (int i = 0; i < parentLayoutPtr->GetChildsCount(); i++){
						layoutProperties = parentLayoutPtr->GetChild(i)->GetLayoutProperties();
						layoutProperties.isFixedLayout = isFixedLayout;
						parentLayoutPtr->GetChild(i)->SetLayoutProperties(layoutProperties);
					}
				}
			}

			SetAllProperties(rootLayoutPtr, dialogProperties, settingsDialog);
		}
	}
}


// private methods

void CLayoutManagerGuiComp::SetAllProperties(ILayout* layout, const ILayout::LayoutProperties& dialogProperties, const CLayoutSettingsDialog& settingsDialog)
{
	if (layout != nullptr){
		ILayout::LayoutProperties properties = layout->GetLayoutProperties();
		if (layout->GetType() == ILayout::LT_NONE){
			if (settingsDialog.AllBorderColor->isChecked()){
				properties.borderColor = dialogProperties.borderColor;
			}
			if (settingsDialog.AllBorderEnabled->isChecked()){
				properties.isBorderEnabled = dialogProperties.isBorderEnabled;
			}
			if (settingsDialog.AllFixedLayouts->isChecked()){
				properties.isFixedLayout = dialogProperties.isFixedLayout;
			}
			if (settingsDialog.AllMinWidth->isChecked()){
				properties.minWidth = dialogProperties.minWidth;
			}
			if (settingsDialog.AllMaxWidth->isChecked()){
				properties.maxWidth = dialogProperties.maxWidth;
			}
			if (settingsDialog.AllMinHeight->isChecked()){
				properties.minHeight = dialogProperties.minHeight;
			}
			if (settingsDialog.AllMaxHeight->isChecked()){
				properties.maxHeight = dialogProperties.maxHeight;
			}
			if (settingsDialog.AllLeftMargin->isChecked()){
				properties.leftMargin = dialogProperties.leftMargin;
			}
			if (settingsDialog.AllRightMargin->isChecked()){
				properties.rightMargin = dialogProperties.rightMargin;
			}
			if (settingsDialog.AllTopMargin->isChecked()){
				properties.topMargin = dialogProperties.topMargin;
			}
			if (settingsDialog.AllBottomMargin->isChecked()){
				properties.bottomMargin = dialogProperties.bottomMargin;
			}
			layout->SetLayoutProperties(properties);
		}
		else{
			if (settingsDialog.AllFixedLayouts->isChecked()){
				properties.isFixedLayout = dialogProperties.isFixedLayout;
			}
			layout->SetLayoutProperties(properties);

			for (int i = 0; i < layout->GetChildsCount(); i++){
				ILayout* childLayout = layout->GetChild(i);
				SetAllProperties(childLayout, dialogProperties, settingsDialog);
			}
		}

	}
}


void CLayoutManagerGuiComp::RemoveAllGuiComponents()
{
	for (GuiObjectPtr guiPtr : m_guiObjects){
		if (guiPtr->IsGuiCreated()){
			guiPtr->DestroyGui();
		}
	}

	m_guiObjects.clear();
}



} // namespace imtgui


