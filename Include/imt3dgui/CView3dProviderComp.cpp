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
	m_rotationCommands("Rotation", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_OTHER),
	m_zoomInCommand("Zoom In", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_ZOOM),
	m_zoomOutCommand("Zoom Out", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_ZOOM),
	m_showGridCommand("Show Grid", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, CG_SHOW_SCENE_ITEMS),
	m_showAxisCommand("Show Axis", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, CG_SHOW_SCENE_ITEMS),
	m_showRulerCommand("Show Ruler", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF, CG_SHOW_SCENE_ITEMS),
	m_setViewFromRightCommand("Right", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromFrontCommand("Front", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromTopCommand("Top", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromLeftCommand("Left", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromBottomCommand("Bottom", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_setViewFromBackCommand("Back", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_resetViewCommand("Reset", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_CAMERA_MODE),
	m_useAntialiasingCommand("Antialiasing", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_ONOFF, CG_SCENE_FLAGS),
	m_useCullFaceCommand("Cull face", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_ONOFF, CG_SCENE_FLAGS),
	m_pointSelectionCommand("Point selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_SELECTION),
	m_boxSelectionCommand("Box selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_SELECTION),
	m_circleSelectionCommand("Circle selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_SELECTION),
	m_clearSelectionCommand("Clear selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_SELECTION_ACTIONS),
	m_allSelectionCommand("Select all", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_SELECTION_ACTIONS),
	m_invertSelectionCommand("Invert selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_SELECTION_ACTIONS),
	m_deleteSelectionCommand("Delete selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR, CG_SELECTION_ACTIONS),
	m_freeRotationCommand("Free rotation", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_ROTATION),
	m_rotationAroundXCommand("Rotation around X", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_ROTATION),
	m_rotationAroundYCommand("Rotation around Y", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_ROTATION),
	m_rotationAroundZCommand("Rotation around Z", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_ROTATION),
	m_viewModeCommand("View Mode", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_VIEW_MODE),
	m_selectionModeCommand("Selection", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CG_VIEW_MODE)
{
	connect(&m_zoomInCommand, SIGNAL(triggered()), this, SLOT(OnZoomIn()));
	connect(&m_zoomOutCommand, SIGNAL(triggered()), this, SLOT(OnZoomOut()));
	connect(&m_showGridCommand, SIGNAL(triggered(bool)), this, SLOT(OnShowGridCommand(bool)));
	connect(&m_showAxisCommand, SIGNAL(triggered(bool)), this, SLOT(OnShowAxisCommand(bool)));
	connect(&m_showRulerCommand, SIGNAL(triggered(bool)), this, SLOT(OnShowRulerCommand(bool)));
	connect(&m_resetViewCommand, SIGNAL(triggered()), this, SLOT(OnViewReset()));
	connect(&m_setViewFromRightCommand, SIGNAL(triggered()), this, SLOT(OnViewFromRight()));
	connect(&m_setViewFromFrontCommand, SIGNAL(triggered()), this, SLOT(OnViewFromFront()));
	connect(&m_setViewFromTopCommand, SIGNAL(triggered()), this, SLOT(OnViewFromTop()));
	connect(&m_setViewFromLeftCommand, SIGNAL(triggered()), this, SLOT(OnViewFromLeft()));
	connect(&m_setViewFromBottomCommand, SIGNAL(triggered()), this, SLOT(OnViewFromBottom()));
	connect(&m_setViewFromBackCommand, SIGNAL(triggered()), this, SLOT(OnViewFromBack()));
	connect(&m_useAntialiasingCommand, SIGNAL(triggered(bool)), this, SLOT(OnUseAntialiasingCommand(bool)));
	connect(&m_useCullFaceCommand, SIGNAL(triggered(bool)), this, SLOT(OnUseCullFaceCommand(bool)));
	connect(&m_pointSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnPointSelectionCommand(bool)));
	connect(&m_boxSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnBoxSelectionCommand(bool)));
	connect(&m_circleSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnCircleSelectionCommand(bool)));
	connect(&m_clearSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnClearSelectionCommand()));
	connect(&m_allSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnAllSelectionCommand()));
	connect(&m_invertSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnInvertSelectionCommand()));
	connect(&m_deleteSelectionCommand, SIGNAL(triggered(bool)), this, SLOT(OnDeleteSelectionCommand()));
	connect(&m_freeRotationCommand, SIGNAL(triggered(bool)), this, SLOT(OnFreeRotationCommand()));
	connect(&m_rotationAroundXCommand, SIGNAL(triggered(bool)), this, SLOT(OnRotationAroundXCommand()));
	connect(&m_rotationAroundYCommand, SIGNAL(triggered(bool)), this, SLOT(OnRotationAroundYCommand()));
	connect(&m_rotationAroundZCommand, SIGNAL(triggered(bool)), this, SLOT(OnRotationAroundZCommand()));
	connect(&m_viewModeCommand, SIGNAL(triggered(bool)), this, SLOT(OnViewModeCommand()));
	connect(&m_selectionModeCommand, SIGNAL(triggered(bool)), this, SLOT(OnSelectionModeCommand()));
}


// reimplemented (imt3dgui::ISceneEventHandler)

void CView3dProviderComp::OnShowGrid(bool /*show*/)
{
}


void CView3dProviderComp::OnShowAxis(bool /*show*/)
{
}


void CView3dProviderComp::OnShowRuler(bool /*show*/)
{
}


void CView3dProviderComp::OnPointSelection(const QPoint& /*point*/, bool /*clearPreviousSelection*/)
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


void CView3dProviderComp::OnAllSelection()
{
}


void CView3dProviderComp::OnInvertSelection()
{
}


void CView3dProviderComp::OnDeleteSelection()
{
}


bool CView3dProviderComp::OnMousePress(QMouseEvent& /*e*/)
{
	return false;
}


bool CView3dProviderComp::OnMouseMove(QMouseEvent& /*e*/)
{
	return false;
}


bool CView3dProviderComp::OnMouseRelease(QMouseEvent& /*e*/)
{
	return false;
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

	// render flags
	bool useAntialiasing = settings.value("RenderHints/Antialiasing", true).toBool();
	widgetPtr->SetRenderHint(COpenGLWidget::RH_ANTIALIASING, useAntialiasing);
	m_useAntialiasingCommand.setChecked(useAntialiasing);

	bool useCullFace = settings.value("RenderHints/CullFace", true).toBool();
	widgetPtr->SetRenderHint(COpenGLWidget::RH_CULLFACE, useCullFace);
	m_useCullFaceCommand.setChecked(useCullFace);

	// camera
	if (m_cameraCompPtr.IsValid()){
		if (settings.contains("Camera/Position")){
			m_cameraCompPtr->MoveTo(settings.value("Camera/Position").value<QVector3D>());
		}

		if (settings.contains("Camera/Rotation")){
			m_cameraCompPtr->RotateTo(settings.value("Camera/Rotation").value<QQuaternion>());
		}
	}
}


void CView3dProviderComp::OnSaveSettings(QSettings& settings) const
{
	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != nullptr);

	settings.beginGroup("RenderHints");
	settings.setValue("Antialiasing", widgetPtr->GetRenderHint(COpenGLWidget::RH_ANTIALIASING));
	settings.setValue("CullFace", widgetPtr->GetRenderHint(COpenGLWidget::RH_CULLFACE));
	settings.endGroup();

	if (m_cameraCompPtr.IsValid()){
		settings.beginGroup("Camera");
		settings.setValue("Position", m_cameraCompPtr->GetPosition());
		settings.setValue("Rotation", m_cameraCompPtr->GetRotation());
		settings.endGroup();
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CView3dProviderComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	imt3dgui::COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != nullptr);

	widgetPtr->SetCamera(m_cameraCompPtr.GetPtr());

	static ChangeSet commandsChanged(ibase::ICommandsProvider::CF_COMMANDS);
	istd::CChangeNotifier changeNotifier(this, &commandsChanged);

	if (*m_showSceneDecorationsCommandsAttrPtr){
		if (*m_showShowGridCommandAttrPtr){
			m_viewCommands.InsertChild(&m_showGridCommand);
			m_showGridCommand.setChecked(true);
		}

		if (*m_showShowAxisCommandAttrPtr){
			m_viewCommands.InsertChild(&m_showAxisCommand);
			m_showAxisCommand.setChecked(true);
		}

		if (*m_showShowRulerCommandAttrPtr){
			m_viewCommands.InsertChild(&m_showRulerCommand);
			m_showRulerCommand.setChecked(true);
		}
	}

	if (*m_showViewCommandsAttrPtr){
		if (*m_showViewModeCommandsAttrPtr){
			if (*m_showViewModeCommandAttrPtr){
				m_viewCommands.InsertChild(&m_viewModeCommand);
			}
			if (*m_showSelectionModeCommandAttrPtr){
				m_viewCommands.InsertChild(&m_selectionModeCommand);
			}
		}

		if (*m_showRotationCommandsAttrPtr){
			if (*m_showFreeRotationCommandAttrPtr){
				m_rotationCommands.InsertChild(&m_freeRotationCommand);
			}
			if (*m_showRotationAroundXCommandAttrPtr){
				m_rotationCommands.InsertChild(&m_rotationAroundXCommand);
			}
			if (*m_showRotationAroundYCommandAttrPtr){
				m_rotationCommands.InsertChild(&m_rotationAroundYCommand);
			}
			if (*m_showRotationAroundYCommandAttrPtr){
				m_rotationCommands.InsertChild(&m_rotationAroundZCommand);
			}

			if (m_rotationCommands.GetChildsCount() > 0){
				m_viewCommands.InsertChild(&m_rotationCommands);
			}
		}

		if (*m_showViewpointCommandsAttrPtr){
			if (*m_showResetViewCommandAttrPtr){
				m_viewCommands.InsertChild(&m_resetViewCommand);
			}
			if (*m_showSetViewFromRightCommandAttrPtr){
				m_viewCommands.InsertChild(&m_setViewFromRightCommand);
			}
			if (*m_showSetViewFromFrontCommandAttrPtr){
				m_viewCommands.InsertChild(&m_setViewFromFrontCommand);
			}
			if (*m_showSetViewFromTopCommandAttrPtr){
				m_viewCommands.InsertChild(&m_setViewFromTopCommand);
			}
			if (*m_showSetViewFromLeftCommandAttrPtr){
				m_viewCommands.InsertChild(&m_setViewFromLeftCommand);
			}
			if (*m_showSetViewFromBottomCommandAttrPtr){
				m_viewCommands.InsertChild(&m_setViewFromBottomCommand);
			}
			if (*m_showSetViewFromBackCommandAttrPtr){
				m_viewCommands.InsertChild(&m_setViewFromBackCommand);
			}
		}

		m_viewModeCommand.setChecked(true);
		m_freeRotationCommand.setChecked(true);
		m_rotationCommands.setIcon(m_freeRotationCommand.icon());
		m_pointSelectionCommand.setChecked(true);

		EnableCommands(COpenGLWidget::ViewMode::VM_VIEW);

		m_rotationAroundXCommand.setShortcut(Qt::Key_X);
		m_rotationAroundYCommand.setShortcut(Qt::Key_Y);
		m_rotationAroundZCommand.setShortcut(Qt::Key_Z);
	}

	if (*m_showZoomCommandsAttrPtr){
		if (*m_showZoomInCommandAttrPtr){
			m_viewCommands.InsertChild(&m_zoomInCommand);
		}
		if (*m_showZoomOutCommandAttrPtr){
			m_viewCommands.InsertChild(&m_zoomOutCommand);
		}
	}

	m_viewCommands.InsertChild(&m_useAntialiasingCommand);
	m_viewCommands.InsertChild(&m_useCullFaceCommand);

	m_useAntialiasingCommand.setChecked(*m_defaultsUseAntialiasingAttrPtr);
	m_useCullFaceCommand.setChecked(*m_defaultsUseCullfaceAttrPtr);

	widgetPtr->SetRenderHint(COpenGLWidget::RH_ANTIALIASING, *m_defaultsUseAntialiasingAttrPtr);
	widgetPtr->SetRenderHint(COpenGLWidget::RH_CULLFACE, *m_defaultsUseCullfaceAttrPtr);
	widgetPtr->SetRenderHint(COpenGLWidget::RH_BLEND, *m_defaultsUseBlendAttrPtr);

	if (m_viewCommands.GetChildsCount() > 0){
		m_rootCommands.InsertChild(&m_viewCommands);
	}

	if (*m_showEditCommandsAttrPtr){
		m_clearSelectionCommand.setShortcut(Qt::Key_Escape);
		m_deleteSelectionCommand.setShortcut(Qt::Key_Delete);
		m_invertSelectionCommand.setShortcut(Qt::Key_Asterisk);
		m_allSelectionCommand.setShortcut(Qt::CTRL + Qt::Key_A);

		if (*m_showPointSelectionCommandAttrPtr){
			m_editCommands.InsertChild(&m_pointSelectionCommand);
		}
		if (*m_showBoxSelectionCommandAttrPtr){
			m_editCommands.InsertChild(&m_boxSelectionCommand);
		}
		if (*m_showCircleSelectionCommandAttrPtr){
			m_editCommands.InsertChild(&m_circleSelectionCommand);
		}
		if (*m_showClearSelectionCommandAttrPtr){
			m_editCommands.InsertChild(&m_clearSelectionCommand);
		}
		if (*m_showAllSelectionCommandAttrPtr){
			m_editCommands.InsertChild(&m_allSelectionCommand);
		}
		if (*m_showInvertSelectionCommandAttrPtr){
			m_editCommands.InsertChild(&m_invertSelectionCommand);
		}
		if (*m_showDeleteSelectionCommandAttrPtr){
			m_editCommands.InsertChild(&m_deleteSelectionCommand);
		}

		m_rootCommands.InsertChild(&m_editCommands);
	}
}


void CView3dProviderComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_zoomInCommand.SetVisuals(tr("Zoom in"), tr("Zoom in"), tr("Zoom in"), QIcon(":/Icons/ViewZoomIn"));
	m_zoomOutCommand.SetVisuals(tr("Zoom out"), tr("Zoom out"), tr("Zoom out"), QIcon(":/Icons/ViewZoomOut"));
	m_showGridCommand.SetVisuals(tr("Show Grid"), tr("Grid"), tr("Show Grid"), QIcon(":/Icons/Grid"));
	m_showAxisCommand.SetVisuals(tr("Show Axis"), tr("Axis"), tr("Show Axis"), QIcon(":/Icons/Axis"));
	m_showRulerCommand.SetVisuals(tr("Show Ruler"), tr("Ruler"), tr("Show Ruler"), QIcon(":/Icons/Ruler"));
	m_resetViewCommand.SetVisuals(tr("Reset View"), tr("Reset"), tr("Reset View"), QIcon(":/Icons/ViewReset"));
	m_setViewFromRightCommand.SetVisuals(tr("Right View"), tr("Right"), tr("Right View"), QIcon(":/Icons/ViewRight"));
	m_setViewFromFrontCommand.SetVisuals(tr("Front View"), tr("Front"), tr("Front View"), QIcon(":/Icons/ViewFront"));
	m_setViewFromTopCommand.SetVisuals(tr("Top View"), tr("Top"), tr("Top View"), QIcon(":/Icons/ViewTop"));
	m_setViewFromLeftCommand.SetVisuals(tr("Left View"), tr("Left"), tr("Left View"), QIcon(":/Icons/ViewLeft"));
	m_setViewFromBottomCommand.SetVisuals(tr("Bottom View"), tr("Bottom"), tr("Bottom View"), QIcon(":/Icons/ViewBottom"));
	m_setViewFromBackCommand.SetVisuals(tr("Back View"), tr("Back"), tr("Back View"), QIcon(":/Icons/ViewBack"));
	m_pointSelectionCommand.SetVisuals(tr("Select Point"), tr("Select Point"), tr("Set Point Selection Mode"), QIcon(":/Icons/PointSelection"));
	m_boxSelectionCommand.SetVisuals(tr("Select Box"), tr("Select Box"), tr("Set Box Selection Mode"), QIcon(":/Icons/BoxSelection"));
	m_circleSelectionCommand.SetVisuals(tr("Select Circle"), tr("Select Circle"), tr("Set Circle Selection Mode"), QIcon(":/Icons/CircleSelection"));
	m_clearSelectionCommand.SetVisuals(tr("Clear Selection"), tr("Clear"), tr("Clear Selection"), QIcon(":/Icons/ClearSelection"));
	m_allSelectionCommand.SetVisuals(tr("Select all"), tr("Select all"), tr("Select all"), QIcon(":/Icons/SelectAll"));
	m_invertSelectionCommand.SetVisuals(tr("Invert Selection"), tr("Invert"), tr("Invert Selection"), QIcon(":/Icons/InvertSelection"));
	m_deleteSelectionCommand.SetVisuals(tr("Delete Selection"), tr("Delete"), tr("Delete Selection"), QIcon(":/Icons/DeleteSelection"));
	m_freeRotationCommand.SetVisuals(tr("Free Rotation"), tr("Free Rotation"), tr("Set Free Rotation Mode"), QIcon(":/Icons/FreeRotation"));
	m_rotationAroundXCommand.SetVisuals(tr("Rotation around X-Axis"), tr("X-Rotation"), tr("Set Rotation around X-Axis Mode"), QIcon(":/Icons/RotationAroundX"));
	m_rotationAroundYCommand.SetVisuals(tr("Rotation around Y-Axis"), tr("Y-Rotation"), tr("Set Rotation around Y-Axis Mode"), QIcon(":/Icons/RotationAroundY"));
	m_rotationAroundZCommand.SetVisuals(tr("Rotation around Z-Axis"), tr("Z-Rotation"), tr("Set Rotation around Z-Axis Mode"), QIcon(":/Icons/RotationAroundZ"));
	m_viewModeCommand.SetVisuals(tr("View"), tr("View Mode"), tr("Set View Mode"), QIcon(":/Icons/View"));
	m_selectionModeCommand.SetVisuals(tr("Selection"), tr("Selection Mode"), tr("Set Selection Mode"), QIcon(":/Icons/Selection"));
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


void CView3dProviderComp::OnShowRulerCommand(bool checked)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->ShowRuler(checked);
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


void CView3dProviderComp::OnPointSelectionCommand(bool /*checked*/)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetSelectionMode(COpenGLWidget::SM_POINT);
}


void CView3dProviderComp::OnBoxSelectionCommand(bool /*checked*/)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetSelectionMode(COpenGLWidget::SM_BOX);
}


void CView3dProviderComp::OnCircleSelectionCommand(bool /*checked*/)
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetSelectionMode(COpenGLWidget::SM_CIRCLE);
}


void CView3dProviderComp::OnClearSelectionCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->ClearSelection();
}


void CView3dProviderComp::OnAllSelectionCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->AllSelection();
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


void CView3dProviderComp::OnFreeRotationCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetRotationMode(COpenGLWidget::RTM_FREE);
	m_rotationCommands.setIcon(m_freeRotationCommand.icon());
}


void CView3dProviderComp::OnRotationAroundXCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetRotationMode(COpenGLWidget::RTM_AROUND_X);
	m_rotationCommands.setIcon(m_rotationAroundXCommand.icon());
}


void CView3dProviderComp::OnRotationAroundYCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetRotationMode(COpenGLWidget::RTM_AROUND_Y);
	m_rotationCommands.setIcon(m_rotationAroundYCommand.icon());
}


void CView3dProviderComp::OnRotationAroundZCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetRotationMode(COpenGLWidget::RTM_AROUND_Z);
	m_rotationCommands.setIcon(m_rotationAroundZCommand.icon());
}


void CView3dProviderComp::OnViewModeCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetViewMode(COpenGLWidget::ViewMode::VM_VIEW);

	EnableCommands(COpenGLWidget::ViewMode::VM_VIEW);
}


void CView3dProviderComp::OnSelectionModeCommand()
{
	COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(GetWidget());
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetViewMode(COpenGLWidget::ViewMode::VM_SELECTION);

	EnableCommands(COpenGLWidget::ViewMode::VM_SELECTION);
}


void CView3dProviderComp::EnableCommands(COpenGLWidget::ViewMode viewMode)
{
	bool isView = (viewMode == COpenGLWidget::ViewMode::VM_VIEW);
	bool isSelection = (viewMode == COpenGLWidget::ViewMode::VM_SELECTION);

	m_zoomInCommand.setEnabled(isView);
	m_zoomOutCommand.setEnabled(isView);
	m_setViewFromRightCommand.setEnabled(isView);
	m_setViewFromFrontCommand.setEnabled(isView);
	m_setViewFromTopCommand.setEnabled(isView);
	m_setViewFromLeftCommand.setEnabled(isView);
	m_setViewFromBottomCommand.setEnabled(isView);
	m_setViewFromBackCommand.setEnabled(isView);
	m_resetViewCommand.setEnabled(isView);
	m_rotationCommands.setEnabled(isView);
	m_freeRotationCommand.setEnabled(isView);
	m_rotationAroundXCommand.setEnabled(isView);
	m_rotationAroundYCommand.setEnabled(isView);
	m_rotationAroundZCommand.setEnabled(isView);

	m_pointSelectionCommand.setEnabled(isSelection);
	m_boxSelectionCommand.setEnabled(isSelection);
	m_circleSelectionCommand.setEnabled(isSelection);
	m_clearSelectionCommand.setEnabled(isSelection);
	m_allSelectionCommand.setEnabled(isSelection);
	m_invertSelectionCommand.setEnabled(isSelection);
	m_deleteSelectionCommand.setEnabled(isSelection);
}


} // namespace imt3dgui


