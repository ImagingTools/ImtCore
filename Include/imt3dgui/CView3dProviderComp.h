#pragma once


// Qt includes
#include <QtWidgets/QWidget>

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


class CView3dProviderCompAttr:
		public iqtgui::TRestorableGuiWrap<iqtgui::TGuiComponentBase<COpenGLWidget>>
{
public:
	typedef iqtgui::TRestorableGuiWrap<iqtgui::TGuiComponentBase<COpenGLWidget>> BaseClass;

	I_BEGIN_BASE_COMPONENT(CView3dProviderCompAttr);
		I_ASSIGN(m_showEditCommandsAttrPtr, "ShowEditCommands", "Show commands for editing the 3D-objects", true, false);
		I_ASSIGN(m_showShowGridCommandAttrPtr, "ShowShowGridCommand", "Enable command for shoe/hide grid", true, false);
		I_ASSIGN(m_showShowAxisCommandAttrPtr, "ShowShowAxisCommands", "Enable command for shoe/hide axis", true, false);
		I_ASSIGN(m_showViewCommandsAttrPtr, "ShowViewCommands", "Show view commands", true, true);
		I_ASSIGN(m_showViewModeCommandsAttrPtr, "ShowViewModeCommands", "Show view mode commands", true, false);
		I_ASSIGN(m_showRotationModeCommandsAttrPtr, "ShowRotationModeCommands", "Show rotation mode commands", true, false);
		I_ASSIGN(m_showViewpointCommandsAttrPtr, "ShowViewpointCommands", "Show view point commands", true, false);
		I_ASSIGN(m_showZoomCommandsAttrPtr, "ShowZoomCommands", "Show zoom-in/zoom-out commands", true, true);
		I_ASSIGN(m_useAntialiasingAttrPtr, "UseAntialiasing", "Use rendering antialiasing", true, true);
		I_ASSIGN(m_useCullfaceAttrPtr, "UseCullFace", "Use cull face mode", true, true);
		I_ASSIGN(m_useBlendAttrPtr, "UseBlend", "Use blending mode", true, true);
		I_ASSIGN(m_defaultShowAxisAttrPtr, "ShowAxis", "If enabled, the axes are shown", true, true);
		I_ASSIGN(m_defaultShowGridAttrPtr, "ShowGrid", "If enabled, the grid is shown", true, true);
	I_END_COMPONENT

protected:
	I_ATTR(bool, m_showEditCommandsAttrPtr);
	I_ATTR(bool, m_showShowGridCommandAttrPtr);
	I_ATTR(bool, m_showShowAxisCommandAttrPtr);
	I_ATTR(bool, m_showViewCommandsAttrPtr);
	I_ATTR(bool, m_showViewModeCommandsAttrPtr);
	I_ATTR(bool, m_showRotationModeCommandsAttrPtr);
	I_ATTR(bool, m_showViewpointCommandsAttrPtr);
	I_ATTR(bool, m_showZoomCommandsAttrPtr);
	I_ATTR(bool, m_useAntialiasingAttrPtr);
	I_ATTR(bool, m_useCullfaceAttrPtr);
	I_ATTR(bool, m_useBlendAttrPtr);
	I_ATTR(bool, m_defaultShowAxisAttrPtr);
	I_ATTR(bool, m_defaultShowGridAttrPtr);
};


class CView3dProviderComp:
			public CView3dProviderCompAttr,
			virtual public ibase::ICommandsProvider,
			virtual public imt3dview::IScene3dProvider,
			virtual public ISceneEventHandler
{
	Q_OBJECT

public:
	typedef CView3dProviderCompAttr BaseClass;

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

	// rendering
	iqtgui::CHierarchicalCommand m_useAntialiasingCommand;
	iqtgui::CHierarchicalCommand m_useCullFaceCommand;

	// selection
	iqtgui::CHierarchicalCommand m_pointSelectionCommand;
	iqtgui::CHierarchicalCommand m_boxSelectionCommand;
	iqtgui::CHierarchicalCommand m_circleSelectionCommand;
	iqtgui::CHierarchicalCommand m_clearSelectionCommand;
	iqtgui::CHierarchicalCommand m_allSelectionCommand;
	iqtgui::CHierarchicalCommand m_invertSelectionCommand;
	iqtgui::CHierarchicalCommand m_deleteSelectionCommand;

	// rotation
	iqtgui::CHierarchicalCommand m_freeRotationCommand;
	iqtgui::CHierarchicalCommand m_rotationAroundXCommand;
	iqtgui::CHierarchicalCommand m_rotationAroundYCommand;
	iqtgui::CHierarchicalCommand m_rotationAroundZCommand;

	// view mode
	iqtgui::CHierarchicalCommand m_viewModeCommand;
	iqtgui::CHierarchicalCommand m_selectionModeCommand;

protected:
	I_REF(imt3dview::IScene3dCamera, m_cameraCompPtr);
};


} // namespace imt3dgui


