#include <imt3dgui/CView3dProviderComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imt3dgui/COpenGLWidget.h>


namespace imt3dgui
{


// public methods

CView3dProviderComp::CView3dProviderComp()
	:m_rootCommands("", 100, ibase::ICommand::CF_GLOBAL_MENU),
	m_viewCommands("View", 100, ibase::ICommand::CF_GLOBAL_MENU),
	m_editCommands("Edit", 100, ibase::ICommand::CF_GLOBAL_MENU),
	m_zoomInCommand("Zoom In", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_ZOOM),
	m_zoomOutCommand("Zoom Out", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_ZOOM),
	m_showGridCommand("Show grid", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, CG_SHOW_SCENE_ITEMS),
	m_showAxisCommand("Show axis", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, CG_SHOW_SCENE_ITEMS),
	m_setViewFromRightCommand("Right", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromFrontCommand("Front", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromTopCommand("Top", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromLeftCommand("Left", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromBottomCommand("Bottom", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromBackCommand("Back", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_resetViewCommand("Reset", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_useAntialiasingCommand("Antialiasing", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_ONOFF, CG_SCENE_FLAGS),
	m_useCullFaceCommand("Cull face", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_ONOFF, CG_SCENE_FLAGS),
	m_boxSelectionCommand("Box selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, CG_SELECTION),
	m_circleSelectionCommand("Circle selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, CG_SELECTION),
	m_clearSelectionCommand("Clear selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_SELECTION),
	m_invertSelectionCommand("Invert selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_SELECTION),
	m_deleteSelectionCommand("Delete selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_SELECTION)
{
	m_zoomInCommand.setIcon(QIcon(":/Icons/ViewZoomIn"));
	m_zoomOutCommand.setIcon(QIcon(":/Icons/ViewZoomOut"));
	m_showGridCommand.setIcon(QIcon(":/Icons/Grid"));
	m_showAxisCommand.setIcon(QIcon(":/Icons/Axis"));
	m_resetViewCommand.setIcon(QIcon(":/Icons/ViewReset"));
	m_setViewFromRightCommand.setIcon(QIcon(":/Icons/ViewRight"));
	m_setViewFromFrontCommand.setIcon(QIcon(":/Icons/ViewFront"));
	m_setViewFromTopCommand.setIcon(QIcon(":/Icons/ViewTop"));
	m_setViewFromLeftCommand.setIcon(QIcon(":/Icons/ViewLeft"));
	m_setViewFromBottomCommand.setIcon(QIcon(":/Icons/ViewBottom"));
	m_setViewFromBackCommand.setIcon(QIcon(":/Icons/ViewBack"));

	m_boxSelectionCommand.setIcon(QIcon(":/Icons/BoxSelection"));
	m_circleSelectionCommand.setIcon(QIcon(":/Icons/CircleSelection"));
	m_clearSelectionCommand.setIcon(QIcon(":/Icons/ClearSelection"));
	m_invertSelectionCommand.setIcon(QIcon(":/Icons/InvertSelection"));
	m_deleteSelectionCommand.setIcon(QIcon(":/Icons/DeleteSelection"));

	connect(&m_zoomInCommand, SIGNAL(triggered()), this, SLOT(OnZoomIn()));
	connect(&m_zoomOutCommand, SIGNAL(triggered()), this, SLOT(OnZoomOut()));
	connect(&m_showGridCommand, SIGNAL(triggered(bool)), this, SLOT(OnShowGridCommand(bool)));
	connect(&m_showAxisCommand, SIGNAL(triggered(bool)), this, SLOT(OnShowAxisCommand(bool)));
	connect(&m_resetViewCommand, SIGNAL(triggered()), this, SLOT(OnViewReset()));
	connect(&m_setViewFromRightCommand, SIGNAL(triggered()), this, SLOT(OnViewFromRight()));
	connect(&m_setViewFromFrontCommand, SIGNAL(triggered()), this, SLOT(OnViewFromFront()));
	connect(&m_setViewFromTopCommand, SIGNAL(triggered()), this, SLOT(OnViewFromTop()));
	connect(&m_setViewFromLeftCommand, SIGNAL(triggered()), this, SLOT(OnViewFromLeft()));
	connect(&m_setViewFromBottomCommand, SIGNAL(triggered()), this, SLOT(OnViewFromBottom()));
	connect(&m_setViewFromBackCommand, SIGNAL(triggered()), this, SLOT(OnViewFromBack()));
	connect(&m_useAntialiasingCommand, SIGNAL(triggered(bool)), this, SLOT(OnUseAntialiasingCommand(bool)));
	connect(&m_useCullFaceCommand, SIGNAL(triggered(bool)), this, SLOT(OnUseCullFaceCommand(bool)));
	connect(&m_boxSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnBoxSelectionCommand(bool)));
	connect(&m_circleSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnCircleSelectionCommand(bool)));
	connect(&m_clearSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnClearSelectionCommand()));
	connect(&m_invertSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnInvertSelectionCommand()));
	connect(&m_deleteSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnDeleteSelectionCommand()));
}


// reimplemented (imt3dgui::ISceneEventHandler)

void CView3dProviderComp::OnCameraPoseChanged(const QQuaternion& /*rotation*/, const QVector3D& /*position*/)
{
}


void CView3dProviderComp::OnShowGrid(bool /*show*/)
{
}


void CView3dProviderComp::OnShowAxis(bool /*show*/)
{
}


void CView3dProviderComp::OnBoxSelection(const QRect& /*rect*/, bool /*clearPreviousSelection*/)
{
}


void CView3dProviderComp::OnCircleSelection(const QRect& /*rect*/, bool /*clearPreviousSelection*/)
{
}


void CView3dProviderComp::OnClearSelection()
{
}


void CView3dProviderComp::OnInvertSelection()
{
}


void CView3dProviderComp::OnDeleteSelection()
{
}


// reimplemented (imt3dview::IScene3dProvider)

imt3dview::IScene3d* CView3dProviderComp::GetScene() const
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	if (widgetPtr != NULL){
		return widgetPtr->GetScene();
	}

	return NULL;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CView3dProviderComp::GetCommands() const
{
	return &m_rootCommands;
}


// protected methods

// reimplemented (iqtgui::TRestorableGuiWrap)

void CView3dProviderComp::OnRestoreSettings(const QSettings& settings)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	imt3dview::IScene3dCamera* cameraPtr = widgetPtr->GetCamera();
	Q_ASSERT(cameraPtr);

	// render flags
	bool useAntialiasing = settings.value("RenderHints/Antialiasing", true).toBool();
	widgetPtr->SetRenderHint(COpenGLWidget::RH_ANTIALIASING, useAntialiasing);
	m_useAntialiasingCommand.setChecked(useAntialiasing);

	bool useCullFace = settings.value("RenderHints/CullFace", true).toBool();
	widgetPtr->SetRenderHint(COpenGLWidget::RH_CULLFACE, useCullFace);
	m_useCullFaceCommand.setChecked(useCullFace);

	// camera
	if (settings.contains("Camera/Position")){
		cameraPtr->MoveTo(settings.value("Camera/Position").value<QVector3D>());
	}

	if (settings.contains("Camera/Rotation")){
		cameraPtr->RotateTo(settings.value("Camera/Rotation").value<QQuaternion>());
	}
}


void CView3dProviderComp::OnSaveSettings(QSettings& settings) const
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	imt3dview::IScene3dCamera* cameraPtr = widgetPtr->GetCamera();
	Q_ASSERT(cameraPtr);

	settings.beginGroup("RenderHints");
	settings.setValue("Antialiasing", widgetPtr->GetRenderHint(COpenGLWidget::RH_ANTIALIASING));
	settings.setValue("CullFace", widgetPtr->GetRenderHint(COpenGLWidget::RH_CULLFACE));
	settings.endGroup();

	settings.beginGroup("Camera");
	settings.setValue("Position", cameraPtr->GetPosition());
	settings.setValue("Rotation", cameraPtr->GetRotation());
	settings.endGroup();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CView3dProviderComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	static ChangeSet commandsChanged(ibase::ICommandsProvider::CF_COMMANDS);
	istd::CChangeNotifier changeNotifier(this, &commandsChanged);

	if (*m_showZoomCommandsAttrPtr){
		m_viewCommands.InsertChild(&m_zoomInCommand);
		m_viewCommands.InsertChild(&m_zoomOutCommand);
	}

	if (*m_showShowGridCommandAttrPtr){
		m_viewCommands.InsertChild(&m_showGridCommand);
	}

	if (*m_showShowAxisCommandAttrPtr){
		m_viewCommands.InsertChild(&m_showAxisCommand);
	}

	if (*m_showViewCommandsAttrPtr){
		m_viewCommands.InsertChild(&m_resetViewCommand);
		m_viewCommands.InsertChild(&m_setViewFromRightCommand);
		m_viewCommands.InsertChild(&m_setViewFromFrontCommand);
		m_viewCommands.InsertChild(&m_setViewFromTopCommand);
		m_viewCommands.InsertChild(&m_setViewFromLeftCommand);
		m_viewCommands.InsertChild(&m_setViewFromBottomCommand);
		m_viewCommands.InsertChild(&m_setViewFromBackCommand);
	}

	m_viewCommands.InsertChild(&m_useAntialiasingCommand);
	m_viewCommands.InsertChild(&m_useCullFaceCommand);

	if (*m_showEditCommandsAttrPtr){
		m_boxSelectionCommand.setChecked(false);
		m_circleSelectionCommand.setChecked(false);
		m_deleteSelectionCommand.setShortcut(Qt::Key_Delete);

		m_editCommands.InsertChild(&m_boxSelectionCommand);
		m_editCommands.InsertChild(&m_circleSelectionCommand);
		m_editCommands.InsertChild(&m_clearSelectionCommand);
		m_editCommands.InsertChild(&m_invertSelectionCommand);
		m_editCommands.InsertChild(&m_deleteSelectionCommand);
		m_rootCommands.InsertChild(&m_editCommands);
	}

	if (m_viewCommands.GetChildsCount() > 0){
		m_rootCommands.InsertChild(&m_viewCommands);
	}

	m_showGridCommand.setChecked(true);
	m_showAxisCommand.setChecked(true);
	m_useAntialiasingCommand.setChecked(true);
	m_useCullFaceCommand.setChecked(true);
}


iqtgui::CHierarchicalCommand& CView3dProviderComp::GetShowGridCommand()
{
	return m_showGridCommand;
}


iqtgui::CHierarchicalCommand& CView3dProviderComp::GetShowAxisCommand()
{
	return m_showAxisCommand;
}


// protected slots

void CView3dProviderComp::OnZoomIn()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->ZoomIn();
}


void CView3dProviderComp::OnZoomOut()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);
	
	widgetPtr->ZoomOut();
}


void CView3dProviderComp::OnShowGridCommand(bool checked)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->ShowGrid(checked);
}


void CView3dProviderComp::OnShowAxisCommand(bool checked)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->ShowAxis(checked);
}


void CView3dProviderComp::OnViewReset()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetCameraView(COpenGLWidget::VD_DEFAULT);
}


void CView3dProviderComp::OnViewFromRight()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetCameraView(COpenGLWidget::VD_RIGHT);
}


void CView3dProviderComp::OnViewFromFront()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetCameraView(COpenGLWidget::VD_FRONT);
}



void CView3dProviderComp::OnViewFromLeft()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetCameraView(COpenGLWidget::VD_LEFT);
}


void CView3dProviderComp::OnViewFromBottom()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetCameraView(COpenGLWidget::VD_BOTTOM);
}


void CView3dProviderComp::OnViewFromBack()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);
	
	widgetPtr->SetCameraView(COpenGLWidget::VD_BACK);
}


void CView3dProviderComp::OnViewFromTop()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetCameraView(COpenGLWidget::VD_TOP);
}


void CView3dProviderComp::OnUseAntialiasingCommand(bool checked)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);
	
	widgetPtr->SetRenderHint(COpenGLWidget::RH_ANTIALIASING, checked);
}


void CView3dProviderComp::OnUseCullFaceCommand(bool checked)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetRenderHint(COpenGLWidget::RH_CULLFACE, checked);
}


void CView3dProviderComp::OnBoxSelectionCommand(bool checked)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetBoxSelection(checked);

	if (checked){
		m_circleSelectionCommand.setChecked(false);
	}
}


void CView3dProviderComp::OnCircleSelectionCommand(bool checked)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetCircleSelection(checked);

	if (checked){
		m_boxSelectionCommand.setChecked(false);
	}
}


void CView3dProviderComp::OnClearSelectionCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->ClearSelection();
}


void CView3dProviderComp::OnInvertSelectionCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->InvertSelection();
}


void CView3dProviderComp::OnDeleteSelectionCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->DeleteSelection();
}


} // namespace imt3dgui


