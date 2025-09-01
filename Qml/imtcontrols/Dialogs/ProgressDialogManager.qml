import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: progressDialog;

	width: Style.sizeHintL;

	title:  qsTr("Progress manager");

	property string message;
	property string totalProgressTitle: qsTr("Total progress");

	canMove: true;
	forceFocus: true;
	contentComp: body;
	notClosingButtons: closeByCancel ? 0 : Enums.cancel;

	property bool hiddenBackground: false;
	property bool noMouseArea: false;
	property real backgroundOpacity: 0.4;

	property string taskId: "";
	property string description: "";
	property real value: 0;

	property bool closeByCancel: false;
	property bool closeWhenFinished: false;
	property bool isCancelling: false;
	property bool isFailed: false;

	property TreeItemModel subtaskModel: TreeItemModel{}

	property bool messageOpened: false;

	signal taskComleted(string id);
	signal taskCanceled(string id);
	signal taskClosed();
	signal subtaskCanceled(string id);

	onRootChanged: {
		if (root){
			if(root.backgroundItem){
				root.backgroundItem.opacity = backgroundOpacity;
			}
		}
	}

	onIsCancellingChanged: {
		setButtonEnabled(Enums.cancel, !isCancelling)
		buttons.visible = !isCancelling;
	}

	onIsFailedChanged: {
		if(isFailed){
			removeButton(Enums.cancel);
			addButton(Enums.ok, qsTr("Ok"), true);
		}
	}

	function addSubtask(id, description, value, isCancellable){
		let index = subtaskModel.insertNewItem();
		subtaskModel.setData("id", id , index);
		subtaskModel.setData("description", description, index);
		subtaskModel.setData("value", value, index);
		subtaskModel.setData("isCancellable", isCancellable , index);
	}

	function setSubtaskData(id, description, value, isCancellable){
		let found = false;
		for(let i = 0; i < subtaskModel.getItemsCount(); i++){
			let id_ = subtaskModel.getData("id", i);
			if(id_ == id){
				subtaskModel.setData("value", value, i);
				found = true;
				break;
			}
		}
		if(!found){
			addSubtask(id, description, value, isCancellable)
		}
	}

	topPanelComp :Component{
		TopPanelDialog{canClose: false}
	}

	Component.onCompleted: {
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	onLocalizationChanged: {
		setButtonName(Enums.cancel, qsTr("Cancel"))
	}

	onFinished: {
		if(buttonId == Enums.cancel){
			//console.log("TASK_CANCELED ", taskId)
			progressDialog.isCancelling = true;
			taskCanceled(taskId);
		}

		if(buttonId == Enums.ok){
			taskClosed();
		}
	}

	onValueChanged: {
		if(value >= 1 && closeWhenFinished){

			// if(taskId !== "" && value >=1 && !messageOpened){
			// 	let text_ = description + " "  + qsTr("completed")
			// 	let param = {"text": description}
			// 	ModalDialogManager.openDialog(messageComp, param)
			// 	messageOpened = true;
			// }

			if(progressDialog.root){
				if(progressDialog.selfComp){
					progressDialog.root.closeByComp(progressDialog.selfComp);
				}
				else {
					progressDialog.root.closeDialog();
				}
			}

			taskComleted(taskId);
		}
	}

	Component{
		id: body;
		Item{
			width: progressDialog.width;
			height: subtaskColumn.y + subtaskColumn.height + (subtaskColumn.height > 0) * 40;

			ProgressBar{
				id: mainProgressBar;

				anchors.top: parent.top;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.topMargin: Style.marginM;
				anchors.leftMargin: Style.marginM;
				anchors.rightMargin: Style.marginM;

				title: progressDialog.totalProgressTitle;
				text:  percent + "%"

				value: progressDialog.value;
			}

			BaseText{
				id: subtaskTitle;

				anchors.top: mainProgressBar.bottom;
				anchors.left: mainProgressBar.left;
				anchors.topMargin: Style.marginM;
				anchors.leftMargin: Style.marginM;
				font.pixelSize: Style.fontSizeL

				text: qsTr("Subtasks") + ":"
				visible: subtaskColumn.height;
			}

			Column{
				id: subtaskColumn;

				anchors.top: subtaskTitle.bottom;
				anchors.left: subtaskTitle.left;
				anchors.right: parent.right;
				anchors.topMargin: Style.marginM;
				anchors.leftMargin: Style.marginM;
				anchors.rightMargin: Style.marginM;
				spacing: Style.marginM;
				Repeater{
					id: subtaskRepeater;

					model: progressDialog.subtaskModel;

					delegate:
						Item{
						width: subtaskColumn.width;
						height: subtaskProgressBar.height;

						ProgressBar{
							id: subtaskProgressBar;

							anchors.verticalCenter: parent.verticalCenter;
							anchors.left: parent.left
							anchors.right: cancelButton.left;
							anchors.rightMargin: Style.marginM;

							width: subtaskColumn.width;

							fontSize: Style.fontSizeM;

							title: model.description !==undefined ? model.description : "";
							text:  percent + "%"

							value: model.value !==undefined ? model.value : 0;
						}
						Button{
							id: cancelButton;

							anchors.right: parent.right;
							anchors.bottom: parent.bottom;
							height: 20;
							text: qsTr("Cancel");
							visible: model.isCancellable !==undefined ? model.isCancellable : true;
							onClicked: {
								progressDialog.subtaskCanceled(model.id)
								parent.visible = false;
							}
						}
					}
				}
			}

			Rectangle{
				anchors.bottom: parent.bottom;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.bottomMargin: Style.marginM;
				anchors.leftMargin: 2*Style.marginM;
				anchors.rightMargin: Style.marginM;

				height: 1;
				color: Style.backgroundColor;
			}
		}
	}

	BaseText{
		id: canceling

		anchors.bottom: parent.bottom;
		anchors.right: parent.right;
		anchors.bottomMargin: Style.marginM + progressDialog.buttons.height/2 - height/2;
		anchors.rightMargin:  Style.marginM;

		visible: progressDialog.isCancelling;

		text: qsTr("Cancelling") + "...";
	}

	Component{
		id: messageComp;
		MessagePopup {
			id: message;

			Component.onDestruction: {
			}
		}
	}

}
