#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TRestorableGuiWrap.h>

// ImtCore includes
#include <imt3dview/IScene3dProvider.h>
#include <imt3dview/IScene3dCamera.h>
#include <imt3dgui/ISceneEventHandler.h>
#include <imt3dgui/COpenGLWidget.h>


namespace imt3dgui
{


class CView3dProviderCompAttrPart1:
		public iqtgui::TRestorableGuiWrap<iqtgui::TGuiComponentBase<COpenGLWidget>>
{
public:
	typedef iqtgui::TRestorableGuiWrap<iqtgui::TGuiComponentBase<COpenGLWidget>> BaseClass;

	I_BEGIN_BASE_COMPONENT(CView3dProviderCompAttrPart1);
		I_ASSIGN(m_showViewCommandsAttrPtr, "ShowViewCommands", "Show view commands", true, true);
		I_ASSIGN(m_showEditCommandsAttrPtr, "ShowEditCommands", "Show commands for editing the 3D-objects", true, true);
		I_ASSIGN(m_showRotationCommandsAttrPtr, "ShowRotationModeCommands", "Show rotation mode commands", true, true);
		I_ASSIGN(m_showZoomCommandsAttrPtr, "ShowZoomCommands", "Show zoom-in/zoom-out commands", true, true);
		I_ASSIGN(m_showSceneDecorationsCommandsAttrPtr, "ShowSceneDecorationsCommands", "Show grid/axis/ruler commands", true, true);
		I_ASSIGN(m_showViewpointCommandsAttrPtr, "ShowViewpointCommands", "Show view point commands", true, true);
		I_ASSIGN(m_showViewModeCommandsAttrPtr, "ShowViewModeCommands", "Show view mode commands", true, true);
		I_ASSIGN(m_showZoomInCommandAttrPtr, "ShowShowZoomInCommand", "Enable command for zoom-in", true, false);
		I_ASSIGN(m_showZoomOutCommandAttrPtr, "ShowShowZoomOutCommand", "Enable command for zoom-out", true, false);
		I_ASSIGN(m_showShowGridCommandAttrPtr, "ShowShowGridCommand", "Enable command for show/hide grid", true, false);
		I_ASSIGN(m_showShowAxisCommandAttrPtr, "ShowShowAxisCommands", "Enable command for show/hide axis", true, false);
		I_ASSIGN(m_showShowRulerCommandAttrPtr, "ShowShowRulerCommands", "Enable command for show/hide ruler", true, false);
	I_END_COMPONENT

protected:
	// show commands
	I_ATTR(bool, m_showViewCommandsAttrPtr);
	I_ATTR(bool, m_showEditCommandsAttrPtr);
	I_ATTR(bool, m_showRotationCommandsAttrPtr);
	I_ATTR(bool, m_showZoomCommandsAttrPtr);
	I_ATTR(bool, m_showSceneDecorationsCommandsAttrPtr);
	I_ATTR(bool, m_showViewpointCommandsAttrPtr);
	I_ATTR(bool, m_showViewModeCommandsAttrPtr);

	// show zoom commands
	I_ATTR(bool, m_showZoomInCommandAttrPtr);
	I_ATTR(bool, m_showZoomOutCommandAttrPtr);

	// show scene decorations commands
	I_ATTR(bool, m_showShowGridCommandAttrPtr);
	I_ATTR(bool, m_showShowAxisCommandAttrPtr);
	I_ATTR(bool, m_showShowRulerCommandAttrPtr);
};

class CView3dProviderCompAttrPart2: public CView3dProviderCompAttrPart1
{
public:
	typedef CView3dProviderCompAttrPart1 BaseClass;

	I_BEGIN_BASE_COMPONENT(CView3dProviderCompAttrPart2);
		I_ASSIGN(m_showSetViewFromRightCommandAttrPtr, "ShowSetViewFromRightCommand", "Enable command for view from right", true, false);
		I_ASSIGN(m_showSetViewFromFrontCommandAttrPtr, "ShowSetViewFromRightCommand", "Enable command for view from front", true, false);
		I_ASSIGN(m_showSetViewFromTopCommandAttrPtr, "ShowSetViewFromTopCommand", "Enable command for view from top", true, false);
		I_ASSIGN(m_showSetViewFromLeftCommandAttrPtr, "ShowSetViewFromLeftCommand", "Enable command for view from left", true, false);
		I_ASSIGN(m_showSetViewFromBottomCommandAttrPtr, "ShowSetViewFromBottomCommand", "Enable command for view from bottom", true, false);
		I_ASSIGN(m_showSetViewFromBackCommandAttrPtr, "ShowSetViewFromBackCommand", "Enable command for view from back", true, false);
		I_ASSIGN(m_showResetViewCommandAttrPtr, "ShowResetViewCommand", "Enable command for reset view", true, false);
		I_ASSIGN(m_showPointSelectionCommandAttrPtr, "ShowPointSelectionCommand", "Enable command for point selection", true, false);
		I_ASSIGN(m_showBoxSelectionCommandAttrPtr, "ShowBoxSelectionCommand", "Enable command for box selection", true, false);
		I_ASSIGN(m_showCircleSelectionCommandAttrPtr, "ShowCircleSelectionCommand", "Enable command for circle selection", true, false);
		I_ASSIGN(m_showClearSelectionCommandAttrPtr, "ShowClearSelectionCommand", "Enable command for clear selection", true, false);
		I_ASSIGN(m_showAllSelectionCommandAttrPtr, "ShowAllSelectionCommand", "Enable command for all selection", true, false);
		I_ASSIGN(m_showInvertSelectionCommandAttrPtr, "ShowInvertSelectionCommand", "Enable command for invert selection", true, false);
		I_ASSIGN(m_showDeleteSelectionCommandAttrPtr, "ShowDeleteSelectionCommand", "Enable command for delete selection", true, false);
	I_END_COMPONENT

protected:
	// show viewpoint commands
	I_ATTR(bool, m_showSetViewFromRightCommandAttrPtr);
	I_ATTR(bool, m_showSetViewFromFrontCommandAttrPtr);
	I_ATTR(bool, m_showSetViewFromTopCommandAttrPtr);
	I_ATTR(bool, m_showSetViewFromLeftCommandAttrPtr);
	I_ATTR(bool, m_showSetViewFromBottomCommandAttrPtr);
	I_ATTR(bool, m_showSetViewFromBackCommandAttrPtr);
	I_ATTR(bool, m_showResetViewCommandAttrPtr);

	// show selection commands
	I_ATTR(bool, m_showPointSelectionCommandAttrPtr);
	I_ATTR(bool, m_showBoxSelectionCommandAttrPtr);
	I_ATTR(bool, m_showCircleSelectionCommandAttrPtr);
	I_ATTR(bool, m_showClearSelectionCommandAttrPtr);
	I_ATTR(bool, m_showAllSelectionCommandAttrPtr);
	I_ATTR(bool, m_showInvertSelectionCommandAttrPtr);
	I_ATTR(bool, m_showDeleteSelectionCommandAttrPtr);
};


class CView3dProviderCompAttrPart3: public CView3dProviderCompAttrPart2
{
public:
	typedef CView3dProviderCompAttrPart2 BaseClass;

	I_BEGIN_BASE_COMPONENT(CView3dProviderCompAttrPart3);
		I_ASSIGN(m_showFreeRotationCommandAttrPtr, "ShowFreeRotationCommand", "Enable command for free rotation", true, false);
		I_ASSIGN(m_showRotationAroundXCommandAttrPtr, "ShowRotationAroundXCommand", "Enable command for rotation around axis X", true, false);
		I_ASSIGN(m_showRotationAroundYCommandAttrPtr, "ShowRotationAroundYCommand", "Enable command for rotation around axis Y", true, false);
		I_ASSIGN(m_showRotationAroundZCommandAttrPtr, "ShowRotationAroundZCommand", "Enable command for rotation around axis Z", true, false);
		I_ASSIGN(m_showViewModeCommandAttrPtr, "ShowViewModeCommand", "Show view mode command", true, false);
		I_ASSIGN(m_showSelectionModeCommandAttrPtr, "ShowSelectionModeCommand", "Show selection mode command", true, false);
		I_ASSIGN(m_defaultsUseAntialiasingAttrPtr, "UseAntialiasing", "Use rendering antialiasing", true, true);
		I_ASSIGN(m_defaultsUseCullfaceAttrPtr, "UseCullFace", "Use cull face mode", true, true);
		I_ASSIGN(m_defaultsUseBlendAttrPtr, "UseBlend", "Use blending mode", true, true);
		I_ASSIGN(m_defaultShowAxisAttrPtr, "ShowAxis", "If enabled, the axes are shown", true, true);
		I_ASSIGN(m_defaultShowGridAttrPtr, "ShowGrid", "If enabled, the grid is shown", true, true);
	I_END_COMPONENT

protected:
	// show rotation commands
	I_ATTR(bool, m_showFreeRotationCommandAttrPtr);
	I_ATTR(bool, m_showRotationAroundXCommandAttrPtr);
	I_ATTR(bool, m_showRotationAroundYCommandAttrPtr);
	I_ATTR(bool, m_showRotationAroundZCommandAttrPtr);

	// show view mode commands
	I_ATTR(bool, m_showViewModeCommandAttrPtr);
	I_ATTR(bool, m_showSelectionModeCommandAttrPtr);

	// defaults
	I_ATTR(bool, m_defaultsUseAntialiasingAttrPtr);
	I_ATTR(bool, m_defaultsUseCullfaceAttrPtr);
	I_ATTR(bool, m_defaultsUseBlendAttrPtr);
	I_ATTR(bool, m_defaultShowAxisAttrPtr);
	I_ATTR(bool, m_defaultShowGridAttrPtr);
};


class CView3dProviderComp:
			public CView3dProviderCompAttrPart3,
			virtual public ibase::ICommandsProvider,
			virtual public imt3dview::IScene3dProvider,
			virtual public ISceneEventHandler
{
	Q_OBJECT

public:
	typedef CView3dProviderCompAttrPart3 BaseClass;

	enum CommandGroups
	{
		CG_ZOOM = 0xeaff,
		CG_CAMERA_MODE,
		CG_SHOW_SCENE_ITEMS,
		CG_SCENE_FLAGS,
		CG_SELECTION,
		CG_SELECTION_ACTIONS,
		CG_ROTATION,
		CG_VIEW_MODE,
		CG_OTHER
	};

	I_BEGIN_COMPONENT(CView3dProviderComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(imt3dview::IScene3dProvider);
		I_REGISTER_SUBELEMENT(Scene);
		I_REGISTER_SUBELEMENT_INTERFACE(Scene, imt3dview::IScene3d, ExtractScene);
		I_ASSIGN(m_cameraCompPtr, "Camera", "Camera", true, "Camera");
	I_END_COMPONENT

	CView3dProviderComp();

	// reimplemented (imt3dgui::ISceneEventHandler)
	void OnShowGrid(bool show) override;
	void OnShowAxis(bool show) override;
	void OnShowRuler(bool show) override;
	void OnPointSelection(const QPoint& point, bool clearPreviousSelection) override;
	void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnClearSelection() override;
	void OnAllSelection() override;
	void OnInvertSelection() override;
	void OnDeleteSelection() override;
	bool OnMousePress(QMouseEvent& e) override;
	bool OnMouseMove(QMouseEvent& e) override;
	bool OnMouseRelease(QMouseEvent& e) override;

	// reimplemented (imt3dview::IScene3dProvider)
	imt3dview::IScene3d* GetScene() const override;

	// reimplemented (ibase::ICommandsProvider)
	const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TRestorableGuiWrap)
	void OnRestoreSettings(const QSettings& settings) override;
	void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiRetranslate() override;

protected:
	iqtgui::CHierarchicalCommand& GetShowGridCommand();
	iqtgui::CHierarchicalCommand& GetShowAxisCommand();

protected Q_SLOTS:
	void OnZoomIn();
	void OnZoomOut();
	void OnShowGridCommand(bool checked);
	void OnShowAxisCommand(bool checked);
	void OnShowRulerCommand(bool checked);
	void OnViewReset();
	void OnViewFromRight();
	void OnViewFromFront();
	void OnViewFromTop();
	void OnViewFromLeft();
	void OnViewFromBottom();
	void OnViewFromBack();
	void OnUseAntialiasingCommand(bool checked);
	void OnUseCullFaceCommand(bool checked);
	void OnPointSelectionCommand(bool checked);
	void OnBoxSelectionCommand(bool checked);
	void OnCircleSelectionCommand(bool checked);
	void OnClearSelectionCommand();
	void OnAllSelectionCommand();
	void OnInvertSelectionCommand();
	void OnDeleteSelectionCommand();
	void OnFreeRotationCommand();
	void OnRotationAroundXCommand();
	void OnRotationAroundYCommand();
	void OnRotationAroundZCommand();
	void OnViewModeCommand();
	void OnSelectionModeCommand();

private:
	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractScene(CView3dProviderComp& component)
	{
		COpenGLWidget* widgetPtr = dynamic_cast<COpenGLWidget*>(component.GetWidget());
		if (widgetPtr != NULL){
			return widgetPtr->GetScene();
		}

		return NULL;
	}

	void EnableCommands(COpenGLWidget::ViewMode viewMode);

private:
	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_viewCommands;
	iqtgui::CHierarchicalCommand m_editCommands;
	iqtgui::CHierarchicalCommand m_rotationCommands;

	// zoom commands
	iqtgui::CHierarchicalCommand m_zoomInCommand;
	iqtgui::CHierarchicalCommand m_zoomOutCommand;

	// show/hide scene decorations
	iqtgui::CHierarchicalCommand m_showGridCommand;
	iqtgui::CHierarchicalCommand m_showAxisCommand;
	iqtgui::CHierarchicalCommand m_showRulerCommand;

	// viewpoint commands
	iqtgui::CHierarchicalCommand m_setViewFromRightCommand;
	iqtgui::CHierarchicalCommand m_setViewFromFrontCommand;
	iqtgui::CHierarchicalCommand m_setViewFromTopCommand;
	iqtgui::CHierarchicalCommand m_setViewFromLeftCommand;
	iqtgui::CHierarchicalCommand m_setViewFromBottomCommand;
	iqtgui::CHierarchicalCommand m_setViewFromBackCommand;
	iqtgui::CHierarchicalCommand m_resetViewCommand;

	// rendering commands
	iqtgui::CHierarchicalCommand m_useAntialiasingCommand;
	iqtgui::CHierarchicalCommand m_useCullFaceCommand;

	// selection commands
	iqtgui::CHierarchicalCommand m_pointSelectionCommand;
	iqtgui::CHierarchicalCommand m_boxSelectionCommand;
	iqtgui::CHierarchicalCommand m_circleSelectionCommand;
	iqtgui::CHierarchicalCommand m_clearSelectionCommand;
	iqtgui::CHierarchicalCommand m_allSelectionCommand;
	iqtgui::CHierarchicalCommand m_invertSelectionCommand;
	iqtgui::CHierarchicalCommand m_deleteSelectionCommand;

	// rotation commands
	iqtgui::CHierarchicalCommand m_freeRotationCommand;
	iqtgui::CHierarchicalCommand m_rotationAroundXCommand;
	iqtgui::CHierarchicalCommand m_rotationAroundYCommand;
	iqtgui::CHierarchicalCommand m_rotationAroundZCommand;

	// view mode commands
	iqtgui::CHierarchicalCommand m_viewModeCommand;
	iqtgui::CHierarchicalCommand m_selectionModeCommand;

protected:
	I_REF(imt3dview::IScene3dCamera, m_cameraCompPtr);
};


} // namespace imt3dgui


