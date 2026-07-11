import QtQuick 2.12
import com.imtcore.imtqml 1.0

/*!
	\qmltype DeclarativeViewBase
	\inqmlmodule imtgui
	\brief Slim declarative base for data-model editors.

	Replacement for the imperative ViewBase contract
	(updateGui()/updateModel()/doUpdateGui()/blockingUpdateModel).
	Editors derived from this base bind their input controls directly
	to the properties of \l model (an ObjectViewModel provided by the
	\l controller). Display uses property bindings; writing happens
	via the ViewModel property setters in the control handlers
	(onEditingFinished, onActivated). No manual field copying and no
	update-blocker logic is needed - echo suppression is handled
	centrally by the ViewModel.

	Typical usage:

	DeclarativeViewBase {
		controller: DataModelController {
			modelId: "User"
		}

		TextInput {
			text: model ? model.username : ""
			onEditingFinished: model.username = text
		}
	}
*/
Item {
	id: declarativeViewBase

	property DataModelController controller: null

	property ObjectViewModel model: controller ? controller.viewModel : null

	property bool isLoading: controller ? controller.isLoading : false

	property string error: controller ? controller.error : ""

	property bool isDirty: controller ? controller.isDirty : false

	property bool readOnly: false

	function submit(){
		if (declarativeViewBase.controller){
			declarativeViewBase.controller.submit()
		}
	}

	function revert(){
		if (declarativeViewBase.controller){
			declarativeViewBase.controller.revert()
		}
	}
}
