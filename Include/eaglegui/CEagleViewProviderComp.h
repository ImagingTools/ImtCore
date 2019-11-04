#pragma once


// Qt includes
#include <QtWidgets/QWidget>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TRestorableGuiWrap.h>

// eagle includes
#include <eagle/IScene3dProvider.h>
#include <eagle/IScene3dCamera.h>

// eaglegui includes
#include <eaglegui/CEagleWidget.h>
#include <eaglegui/ISceneEventHandler.h>


namespace eaglegui
{


class CEagleViewProviderComp:
			public iqtgui::TRestorableGuiWrap<iqtgui::TGuiComponentBase<CEagleWidget>>,
			public ibase::ICommandsProvider,
			public eagle::IScene3dProvider,
			public ISceneEventHandler
{
	Q_OBJECT

public:
	typedef iqtgui::TRestorableGuiWrap<iqtgui::TGuiComponentBase<CEagleWidget>> BaseClass;

	enum CommandGroups
	{
		CG_ZOOM = 0xeaff,
		CG_CAMERA_MODE,
		CG_SHOW_SCENE_ITEMS,
		CG_SCENE_FLAGS,
		CG_SELECTION
	};

	I_BEGIN_COMPONENT(CEagleViewProviderComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(eagle::IScene3dProvider);
		I_REGISTER_SUBELEMENT(Camera);
		I_REGISTER_SUBELEMENT_INTERFACE(Camera, eagle::IScene3dCamera, ExtractCamera);
		I_REGISTER_SUBELEMENT(Scene);
		I_REGISTER_SUBELEMENT_INTERFACE(Scene, eagle::IScene3d, ExtractScene);
		I_ASSIGN(m_showEditCommandsAttrPtr, "ShowEditCommands", "Show commands for editing the 3D-objects", true, false);
		I_ASSIGN(m_showShowGridCommandAttrPtr, "ShowShowGridCommand", "Enable command for shoe/hide grid", true, false);
		I_ASSIGN(m_showShowAxisCommandAttrPtr, "ShowShowAxisCommands", "Enable command for shoe/hide axis", true, false);
		I_ASSIGN(m_showViewCommandsAttrPtr, "ShowViewCommands", "Show view commands", true, true);
		I_ASSIGN(m_showZoomCommandsAttrPtr, "ShowZoomCommands", "Show zoom-in/zoom-out commands", true, true);
	I_END_COMPONENT

	CEagleViewProviderComp();

	// reimplemented (eaglegui::ISceneEventHandler
	void OnCameraPoseChanged(const QQuaternion& rotation, const QVector3D& position) override;
	void OnShowGrid(bool show) override;
	void OnShowAxis(bool show) override;
	void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnClearSelection() override;
	void OnInvertSelection() override;
	void OnDeleteSelection() override;

	// reimplemented (eagle::IScene3dProvider)
	eagle::IScene3d* GetScene() const override;

	// reimplemented (ibase::ICommandsProvider)
	const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TRestorableGuiWrap)
	void OnRestoreSettings(const QSettings& settings) override;
	void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

protected:
	iqtgui::CHierarchicalCommand& GetShowGridCommand();
	iqtgui::CHierarchicalCommand& GetShowAxisCommand();

protected Q_SLOTS:
	void OnZoomIn();
	void OnZoomOut();
	void OnShowGridCommand(bool checked);
	void OnShowAxisCommand(bool checked);
	void OnViewReset();
	void OnViewFromRight();
	void OnViewFromFront();
	void OnViewFromTop();
	void OnViewFromLeft();
	void OnViewFromBottom();
	void OnViewFromBack();
	void OnUseAntialiasingCommand(bool checked);
	void OnUseCullFaceCommand(bool checked);
	void OnBoxSelectionCommand(bool checked);
	void OnCircleSelectionCommand(bool checked);
	void OnClearSelectionCommand();
	void OnInvertSelectionCommand();
	void OnDeleteSelectionCommand();

private:
	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractCamera(CEagleViewProviderComp& component)
	{
		CEagleWidget* eagleWidgetPtr = dynamic_cast<CEagleWidget*>(component.GetWidget());
		if (eagleWidgetPtr != NULL){
			return eagleWidgetPtr->GetCamera();
		}

		return NULL;
	}

	template <class InterfaceType>
	static InterfaceType* ExtractScene(CEagleViewProviderComp& component)
	{
		CEagleWidget* eagleWidgetPtr = dynamic_cast<CEagleWidget*>(component.GetWidget());
		if (eagleWidgetPtr != NULL){
			return eagleWidgetPtr->GetScene();
		}

		return NULL;
	}

private:
	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_viewCommands;
	iqtgui::CHierarchicalCommand m_editCommands;

	// zoom commands
	iqtgui::CHierarchicalCommand m_zoomInCommand;
	iqtgui::CHierarchicalCommand m_zoomOutCommand;

	// show/hide scene decorations
	iqtgui::CHierarchicalCommand m_showGridCommand;
	iqtgui::CHierarchicalCommand m_showAxisCommand;

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
	iqtgui::CHierarchicalCommand m_boxSelectionCommand;
	iqtgui::CHierarchicalCommand m_circleSelectionCommand;
	iqtgui::CHierarchicalCommand m_clearSelectionCommand;
	iqtgui::CHierarchicalCommand m_invertSelectionCommand;
	iqtgui::CHierarchicalCommand m_deleteSelectionCommand;

	I_ATTR(bool, m_showEditCommandsAttrPtr);
	I_ATTR(bool, m_showShowGridCommandAttrPtr);
	I_ATTR(bool, m_showShowAxisCommandAttrPtr);
	I_ATTR(bool, m_showViewCommandsAttrPtr);
	I_ATTR(bool, m_showZoomCommandsAttrPtr);
};


} // namespace eaglegui


