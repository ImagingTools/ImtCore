#include <imtgui/CLayoutManagerGuiComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <QtGui/QIcon>

// ACF includes
#include <istd/CChangeGroup.h>
#include <iser/CXmlStringWriteArchive.h>
#include <iser/CXmlStringReadArchive.h>


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
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CLayoutManagerGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


bool CLayoutManagerGuiComp::Serialize(iser::IArchive& archive)
{
	return m_layoutWidgetPtr->Serialize(archive);
//	return true;
}


// protected methods

QWidget* CLayoutManagerGuiComp::CreateCustomLayoutWidget(ILayout* layout)
{
	QWidget* retVal = nullptr;
	if (layout->GetType() == ILayout::LT_NONE){
		CCustomLayoutWidget* customLayoutWidgetPtr = m_layoutWidgetPtr->createCustomWidget();
		customLayoutWidgetPtr->SetId(layout->GetLayoutId());
		customLayoutWidgetPtr->SetIcon(layout->GetIcon());
		customLayoutWidgetPtr->SetTitleAlign(layout->GetTitleAlign());
		customLayoutWidgetPtr->SetName(layout->GetTitle());
		QByteArray viewId = layout->GetViewId();
		if (!viewId.isEmpty()){
			int index = m_guiViewIdMultiAttrPtr.FindValue(viewId);
			if (index < 0){
				//		m_layoutWidgetPtr->SetWidgetToItem(id, QByteArray(), nullptr);
			}
			else{
				istd::TSmartPtr<iqtgui::IGuiObject> newWidgetPtr(m_guiViewMultiFactCompPtr.CreateInstance(index));
				if (newWidgetPtr->CreateGui(nullptr)){
					customLayoutWidgetPtr->SetWidget(newWidgetPtr->GetWidget());
					customLayoutWidgetPtr->SetViewId(viewId);
					//m_layoutWidgetPtr->SetWidgetToItem(id, viewId, newWidgetPtr->GetWidget());
					//m_createdViewMap.insert(layout->GetLayoutId(), newWidgetPtr);
				}
			}

		}

		retVal = customLayoutWidgetPtr;
	}
	else{
		QSplitter* splitter = new QSplitter();
		retVal = splitter;
		if (layout->GetType() == ILayout::LT_HORIZONTAL_SPLITTER){
			splitter->setOrientation(Qt::Horizontal);
		}
		else{
			splitter->setOrientation(Qt::Vertical);
		}
		for (int i = 0; i < layout->GetChildsCount(); i++){
			ILayout* childLayoutPtr = layout->GetChild(i);
			if (childLayoutPtr != nullptr){
				splitter->addWidget(CreateCustomLayoutWidget(childLayoutPtr));
			}
			//splitter->setSizes(layout->GetSizes());
		}
	}

	return retVal;
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CLayoutManagerGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_layoutWidgetPtr != nullptr){
		m_layoutWidgetPtr->ClearAll();

		m_layoutWidgetPtr->layout()->addWidget(CreateCustomLayoutWidget(GetObservedObject()));
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
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitSplitVertical(const QByteArray&)), this, SLOT(OnSplitVertical(const QByteArray&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitSplitHorizontal(const QByteArray&)), this, SLOT(OnSplitHorizontal(const QByteArray&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeIcon(const QByteArray&)), this, SLOT(OnChangeIcon(const QByteArray&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeTitle(const QByteArray&, const QString&)), this, SLOT(OnChangeTitle(const QByteArray&, const QString&)), Qt::DirectConnection);
	QObject::connect(m_layoutWidgetPtr, SIGNAL(EmitChangeAlignTitle(const QByteArray&, const ILayout::AlignType&)), this, SLOT(OnChangeAlignTitle(const QByteArray&, const ILayout::AlignType&)), Qt::DirectConnection);

	// check views attributes
	Q_ASSERT_X(m_guiViewIdMultiAttrPtr.IsValid(), "CLayoutManagerGuiComp", "attribute ViewIds should be set");
	Q_ASSERT_X(m_guiViewMultiFactCompPtr.IsValid(), "CLayoutManagerGuiComp", "attribute ViewFactories should be set");
	Q_ASSERT_X(m_guiViewNameMultiAttrPtr.IsValid(), "CLayoutManagerGuiComp", "attribute ViewNames should be set");
	Q_ASSERT_X((m_guiViewIdMultiAttrPtr.GetCount() == m_guiViewMultiFactCompPtr.GetCount()) &&
		(m_guiViewNameMultiAttrPtr.GetCount() == m_guiViewMultiFactCompPtr.GetCount()), "CLayoutManagerGuiComp", "attributes ViewIds, ViewNames and ViewFactories should have the same count");

	// add undo manager commands provider
	if (m_commandsProviderCompPtr.IsValid()){
		const iqtgui::CHierarchicalCommand *commands = dynamic_cast<const iqtgui::CHierarchicalCommand*>(m_commandsProviderCompPtr->GetCommands());
		for (int i = 0; i < commands->GetChildsCount(); i++)
		{
		    iqtgui::CHierarchicalCommand *command = dynamic_cast<iqtgui::CHierarchicalCommand*>(commands->GetChild(i));
			if (command != nullptr){
				m_rootCommands.InsertChild(command);
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


void CLayoutManagerGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	// File commands emptyIcon
	m_startEndEditModeCommand.SetVisuals(tr("Edit Mode"), tr("Edit Mode"), tr("EditMode"), QIcon(":/Icons/Edit"));
	m_clearCommand.SetVisuals(tr("Clear All"), tr("Clear All"), tr("ClearAll"), QIcon(":/Icons/ClearLayouts"));
	m_loadCommand.SetVisuals(tr("Load"), tr("Load"), tr("Load"), QIcon(":/Icons/Load"));
	m_saveCommand.SetVisuals(tr("Save"), tr("Save"), tr("Save"), QIcon(":/Icons/Save"));

}


void CLayoutManagerGuiComp::OnClearAll()
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	if (m_layoutWidgetPtr != nullptr){
		QMessageBox msgBox;
		msgBox.setText(tr("Do you want clear All views?"));
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);

		int ret = msgBox.exec();
		if (ret == QMessageBox::Ok){
			rootLayoutPtr->Clear();
		}
	}

}


void CLayoutManagerGuiComp::OnLoad()
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	QString fileName = QFileDialog::getOpenFileName(GetWidget(), tr("Open File"), QString(), QString("*.layout"));
	if (!fileName.isEmpty()){
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly)){
			iser::CXmlStringReadArchive archive(file.readAll());
			if ((rootLayoutPtr != nullptr) && rootLayoutPtr->Serialize(archive)){
				// error
			}

			file.close();
		}
	}
}


void CLayoutManagerGuiComp::OnSave()
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	QString fileName = QFileDialog::getSaveFileName(GetWidget(), tr("Save File"), QString(), QString("*.layout"));
	if (!fileName.isEmpty()){
		iser::CXmlStringWriteArchive archive;
		if (rootLayoutPtr != nullptr && rootLayoutPtr->Serialize(archive)){
			QFile file(fileName);
			if (file.open(QIODevice::WriteOnly)){
				file.write(archive.GetString());
				file.close();
			}
		}
	}
}


void CLayoutManagerGuiComp::OnChangeTitle(const QByteArray& id, const QString& title)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	if (rootLayoutPtr != nullptr){
		ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
		if (childLayoutPtr != nullptr){
			istd::CChangeGroup changeGroup(childLayoutPtr);

			childLayoutPtr->SetTitle(title);
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


void CLayoutManagerGuiComp::OnSplitVertical(const QByteArray& id)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		childLayoutPtr->SetType(ILayout::LT_VERTICAL_SPLITTER);
	}
}


void CLayoutManagerGuiComp::OnSplitHorizontal(const QByteArray& id)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
	if (childLayoutPtr != nullptr){
		childLayoutPtr->SetType(ILayout::LT_HORIZONTAL_SPLITTER);
	}
}


void CLayoutManagerGuiComp::OnDelete()
{
	istd::IChangeable::ChangeSet changeSet(0);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (m_layoutWidgetPtr != nullptr){
		m_layoutWidgetPtr->RemoveLayout(m_activeId);
	}
}


void CLayoutManagerGuiComp::OnDeleteWidget(const QByteArray& id)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	delete rootLayoutPtr->RemoveChild(id);
}


void CLayoutManagerGuiComp::OnClear()
{
	OnClearWidget(m_activeId);
}


void CLayoutManagerGuiComp::OnAddWidget()
{
	//QAction *action = qobject_cast<QAction*> (sender());
	//if (action != nullptr){
	//	int index = action->data().toInt();
	//	istd::TSmartPtr<iqtgui::IGuiObject> newWidgetPtr(m_guiViewMultiFactCompPtr.CreateInstance(index));
	//	if (newWidgetPtr->CreateGui(nullptr)){
	//		m_layoutWidgetPtr->SetWidgetToItem(m_activeId, newWidgetPtr->GetWidget());
	//		m_createdViewMap.insert(m_activeId, newWidgetPtr);
	//	}
	//}
}


void CLayoutManagerGuiComp::OnAddWidget(const QByteArray& id, int index)
{
	ILayout* rootLayoutPtr = GetObservedObject();
	Q_ASSERT(rootLayoutPtr != nullptr);

	if (index < 0){
		//m_layoutWidgetPtr->SetWidgetToItem(id, QByteArray(), nullptr);
	}
	else{
		QByteArray viewId = m_guiViewIdMultiAttrPtr[index];

		ILayout* childLayoutPtr = rootLayoutPtr->FindChild(id);
		if (childLayoutPtr != nullptr){
			childLayoutPtr->SetViewId(viewId);
		}
	}


	//if (index < 0){
	//	m_layoutWidgetPtr->SetWidgetToItem(id, QByteArray(), nullptr);
	//}
	//else{
	//	istd::IChangeable::ChangeSet changeSet(0);
	//	istd::CChangeNotifier changeNotifier(this, &changeSet);

	//	istd::TSmartPtr<iqtgui::IGuiObject> newWidgetPtr(m_guiViewMultiFactCompPtr.CreateInstance(index));
	//	if (newWidgetPtr->CreateGui(nullptr)){
	//		QByteArray viewId = m_guiViewIdMultiAttrPtr[index];
	//		m_layoutWidgetPtr->SetWidgetToItem(id, viewId, newWidgetPtr->GetWidget());
	//		m_createdViewMap.insert(id, newWidgetPtr);
	//	}
	//}

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


void CLayoutManagerGuiComp::OnDropWidget(QByteArray id, QDropEvent* eventPtr)
{
	//const QMimeData* mimeDataPtr = eventPtr->mimeData();
	//if (mimeDataPtr != nullptr){
	//	QByteArray mimeData = mimeDataPtr->data("widget-item");
	//	qDebug() << "drop event for id " << id << " with data = " << mimeData;

	//	// gui views part
	//	if (m_guiViewIdMultiAttrPtr.IsValid() && m_guiViewMultiFactCompPtr.IsValid() && m_guiViewNameMultiAttrPtr.IsValid()){
	//		int minCount = qMin(m_guiViewIdMultiAttrPtr.GetCount(), m_guiViewMultiFactCompPtr.GetCount());
	//		minCount = qMin(minCount, m_guiViewNameMultiAttrPtr.GetCount());
	//		for (int i = 0; i < minCount; ++i){
	//			if ((m_layoutWidgetPtr != nullptr) && (m_guiViewIdMultiAttrPtr[i] == mimeData)){
	//				istd::TSmartPtr<iqtgui::IGuiObject> newWidgetPtr(m_guiViewMultiFactCompPtr.CreateInstance(i));
	//				if (newWidgetPtr->CreateGui(nullptr)){
	//					m_layoutWidgetPtr->SetWidgetToItem(id, newWidgetPtr->GetWidget());
	//					m_createdViewMap.insert(id, newWidgetPtr);
	//				}
	//			}
	//		}
	//	}
	//}
}


void CLayoutManagerGuiComp::OnOpenMenu(QByteArray id, QMouseEvent* eventPtr)
{
	m_activeId = id;
	QMenu menu(GetQtWidget());
//	menu.addAction("Clear All", this, &CLayoutManagerComp::OnClearAll);
	//menu.addAction("Change Name", this, &CLayoutManagerComp::OnChangeName);
	//menu.addAction("Split Vertical", this, &CLayoutManagerComp::OnSplitVertical);
	//menu.addAction("Split Horizontal", this, &CLayoutManagerComp::OnSplitHorizontal);
	//menu.addAction("Delete", this, &CLayoutManagerComp::OnDelete);
	if (m_createdViewMap.contains(id)){
		menu.addAction("Clear", this, &CLayoutManagerGuiComp::OnClear);
	}
	menu.addSeparator();

	for (int i = 0; i < m_guiViewOptionsManager.GetOptionsCount(); i++){
		QString name = m_guiViewOptionsManager.GetOptionName(i);
		//QAction *action = menu.addAction("Insert " + name, this, &CLayoutManagerComp::OnAddWidget);
		//action->setData(i);
	}
	menu.exec(eventPtr->globalPos());
}


void CLayoutManagerGuiComp::OnClearWidget(QByteArray id)
{
	Q_ASSERT(m_createdViewMap.contains(id));
	m_layoutWidgetPtr->SetWidgetToItem(m_activeId, QByteArray(), nullptr);
	m_createdViewMap[id]->DestroyGui();
	m_createdViewMap.remove(id);
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
			}
			else{
				m_layoutWidgetPtr->SetViewMode(CHierarchicalLayoutWidget::VM_NORMAL);
				m_clearCommand.setVisible(false);
				m_loadCommand.setVisible(false);
				m_saveCommand.setVisible(false);
			}
		}
	}
}


} // namespace imtgui


