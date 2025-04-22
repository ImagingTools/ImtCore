import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Dialog {
	id: progressDialog;
	
	width: 500;
	
	title:  qsTr("Progress manager");
	
	property string message;
	
	canMove: true;
	forceFocus: true;
	contentComp: body;

	property string taskId: "";
	property string description: "";
	property real value: 0;

	property TreeItemModel subtaskModel: TreeItemModel{}

	property bool messageOpened: false;

	signal taskComleted(string id);
	signal taskCanceled(string id);
	signal subtaskCanceled(string id);

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
			console.log("TASK_CANCELED ", taskId)
			taskCanceled(taskId);
		}

	}
	
	onValueChanged: {
		if(value >= 1){

			// if(taskId !== "" && value >=1 && !messageOpened){
			// 	let text_ = description + " "  + qsTr("completed")
			// 	let param = {"text": description}
			// 	ModalDialogManager.openDialog(messageComp, param)
			// 	messageOpened = true;
			// }

			root.closeDialog()
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
				anchors.topMargin: Style.sizeMainMargin;
				anchors.leftMargin: Style.sizeMainMargin;
				anchors.rightMargin: Style.sizeMainMargin;

				title: qsTr("Total progress");
				text:  percent + "%"

				value: progressDialog.value;
			}

			BaseText{
				id: subtaskTitle;

				anchors.top: mainProgressBar.bottom;
				anchors.left: mainProgressBar.left;
				anchors.topMargin: Style.sizeMainMargin;
				anchors.leftMargin: Style.sizeMainMargin;
				font.pixelSize: Style.fontSizeLarge

				text: qsTr("Subtasks") + ":"
				visible: subtaskColumn.height;
			}

				Column{
					id: subtaskColumn;

					anchors.top: subtaskTitle.bottom;
					anchors.left: subtaskTitle.left;
					anchors.right: parent.right;
					anchors.topMargin: Style.sizeMainMargin;
					anchors.leftMargin: Style.sizeMainMargin;
					anchors.rightMargin: Style.sizeMainMargin;
					spacing: Style.sizeMainMargin;
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
								anchors.rightMargin: Style.sizeMainMargin;

								width: subtaskColumn.width;

								fontSize: Style.fontSizeNormal;

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
					anchors.bottomMargin: Style.sizeMainMargin;
					anchors.leftMargin: 2*Style.sizeMainMargin;
					anchors.rightMargin: Style.sizeMainMargin;

					height: 1;
					color: Style.backgroundColor;
				}
		}
	}

	Component{
		id: messageComp;
		MessagePopup {
			id: message;

			Component.onDestruction: {
				//progressDialog.root.closeDialog()
			}
		}
	}

}
