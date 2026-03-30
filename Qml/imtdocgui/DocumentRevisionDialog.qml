import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

Dialog {
	id: documentRevisionDialog;
	title: qsTr("Revision Manager");
	canMove: false;

	notClosingButtons: Enums.no | Enums.save;

	property string collectionId;
	property string documentId;
	property int selectedRevision: -1;
	property int currentRevision: -1;
	property BaseModel revisionsModel: BaseModel {};

	Component.onCompleted: {
		if (PermissionsController.checkPermission("RestoreRevision")){
			addButton(Enums.save, qsTr("Set this revision"), false)
		}

		if (PermissionsController.checkPermission("DeleteRevision")){
			addButton(Enums.no, qsTr("Delete revision"), false)
		}

		addButton(Enums.cancel, qsTr("Close"), true)
	}

	signal revisionModelReceived()
	signal receiveRevisionModelStarted()
	signal receiveRevisionModelFailed()

	onFinished: {
		if (buttonId == Enums.save){
			setRevisionRequest.send();
		}
		else if (buttonId == Enums.no){
			ModalDialogManager.openDialog(removeRevisionConfirmDialog, {});
		}
	}

	function getRevisionList(){
		receiveRevisionModelStarted()
		request.send();
	}

	function getHeaders(){
		return {};
	}

	GqlSdlRequestSender {
		id: request;
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_getRevisionInfoList;
		inputObjectComp: Component {
			GetRevisionInfoListInput {
				m_documentId: documentRevisionDialog.documentId;
				m_collectionId: documentRevisionDialog.collectionId;
			}
		}

		sdlObjectComp: Component { RevisionInfoList {
				onFinished: {
					documentRevisionDialog.revisionsModel = m_revisions;
					documentRevisionDialog.currentRevision = m_activeRevision;

					documentRevisionDialog.revisionModelReceived()
				}
			}
		}

		function getHeaders(){
			return documentRevisionDialog.getHeaders();
		}
	}

	GqlSdlRequestSender {
		id: deleteRevisionRequest;
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_deleteRevision;
		inputObjectComp: Component {
			DeleteRevisionInput {
				m_objectId: documentRevisionDialog.documentId;
				m_revision: documentRevisionDialog.selectedRevision;
				m_collectionId: documentRevisionDialog.collectionId;
			}
		}

		sdlObjectComp: Component { DeleteRevisionResponse {
				onFinished: {
					documentRevisionDialog.getRevisionList()
					ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully delete"));
				}
			}
		}

		function getHeaders(){
			return documentRevisionDialog.getHeaders();
		}
	}

	GqlSdlRequestSender {
		id: setRevisionRequest;
		requestType: 1;
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_restoreRevision;
		inputObjectComp: Component {
			RestoreRevisionInput {
				m_objectId: documentRevisionDialog.documentId;
				m_revision: documentRevisionDialog.selectedRevision;
				m_collectionId: documentRevisionDialog.collectionId;
			}
		}

		sdlObjectComp: Component { RestoreRevisionResponse {
				onFinished: {
					documentRevisionDialog.getRevisionList()
					ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully set"));
				}
			}
		}

		function getHeaders(){
			return {};
		}
	}

	Component {
		id: removeRevisionConfirmDialog;
		MessageDialog {
			width: Style.sizeHintM;
			title: qsTr("Remove revision");
			message: qsTr("Delete the selected revision ?");
			onFinished: {
				if (buttonId == Enums.yes){
					deleteRevisionRequest.send()
				}
			}
		}
	}

	contentComp: Component {
		Item {
			id: contentItem;
			width: Style.sizeHintXXL;
			height: Style.sizeHintXL;

			Connections {
				target: documentRevisionDialog;

				function onStarted(){
					documentRevisionDialog.getRevisionList()
				}

				function onReceiveRevisionModelStarted(){
					loading.start()
				}

				function onReceiveRevisionModelFailed(){
					loading.stop()
				}

				function onRevisionModelReceived(){
					loading.stop()
				}
			}

			Component {
				id: isActiveColumnDelegateComp;
				TableCellDelegateBase {
					id: cellDelegate

					Image {
						id: image;
						anchors.verticalCenter: parent.verticalCenter;
						anchors.left: parent.left;
						anchors.leftMargin: Style.marginM;
						width: Style.iconSizeM;
						height: width;
						source: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
						sourceSize.width: width;
						sourceSize.height: height;
					}

					onReused: {
						if (!rowDelegate){
							return
						}

						if (rowIndex >= 0){
							let isActive = table.elements.get(rowIndex).item.m_isActive;
							image.visible = isActive;
						}
					}
				}
			}

			Item {
				id: splitView;
				anchors.fill: parent
				anchors.margins: Style.marginM

				Table {
					id: table;
					anchors.top: parent.top
					width: parent.width;
					height: Style.sizeHintM;
					elements: documentRevisionDialog.revisionsModel

					onElementsChanged: {
						table.resetSelection();
					}

					onHeadersChanged: {
						setColumnContentById("isActive", isActiveColumnDelegateComp);
					}

					onSelectionChanged: {
						documentRevisionDialog.buttons.setButtonState(Enums.save, false);
						documentRevisionDialog.buttons.setButtonState(Enums.no, false);

						let isEnabled = selection.length === 1;
						if (isEnabled){
							if (selection.length > 0){
								let index = selection[0]
								let selectedRevision = table.elements.get(index).item.m_revision;
								if (selectedRevision !== documentRevisionDialog.currentRevision){
									documentRevisionDialog.selectedRevision = selectedRevision;
									documentRevisionDialog.buttons.setButtonState(Enums.save, true);
									documentRevisionDialog.buttons.setButtonState(Enums.no, true);
								}

								descriptionTextField.text = table.elements.get(index).item.m_description;
							}
						}
					}

					TreeItemModel {
						id: revisionHeadersModel
						Component.onCompleted: {
							updateHeaders()
							table.headers = revisionHeadersModel
						}
				
						function updateHeaders(){
							revisionHeadersModel.clear()
				
							let index = revisionHeadersModel.insertNewItem();
							revisionHeadersModel.setData("id", "user", index)
							revisionHeadersModel.setData("name", qsTr("User"), index)
				
							index = revisionHeadersModel.insertNewItem();
							revisionHeadersModel.setData("id", "revision", index)
							revisionHeadersModel.setData("name", qsTr("Revision"), index)
				
							index = revisionHeadersModel.insertNewItem();
							revisionHeadersModel.setData("id", "isActive", index)
							revisionHeadersModel.setData("name", qsTr("Active"), index)
				
							index = revisionHeadersModel.insertNewItem();
							revisionHeadersModel.setData("id", "timestamp", index)
							revisionHeadersModel.setData("name", qsTr("Time"), index)
						}
					}
				}

				Rectangle {
					id: descriptionItem;
					anchors.top: table.bottom
					width: parent.width;
					height: Style.sizeHintXXS;
					clip: true
					color: Style.backgroundColor2

					Text {
						id: descriptionTitle;
						anchors.top: parent.top;
						anchors.topMargin: Style.marginM;
						color: Style.buttonTextColor;
						font.family: Style.fontFamilyBold;
						font.pixelSize: Style.fontSizeM;
						font.bold: true;
						text: qsTr("Description");
					}

					Rectangle {
						id: background;
						anchors.top: descriptionTitle.bottom;
						anchors.topMargin: Style.marginM;
						anchors.bottom: parent.bottom;
						width: parent.width;
						border.width: 1;
						border.color: Style.borderColor2;
						radius: 3;
					}

					CustomScrollbar {
						id: scrollbar;
						z: parent.z + 1;
						anchors.right: parent.right;
						anchors.top: parent.top;
						anchors.bottom: parent.bottom;
						secondSize: Style.marginXS;
						targetItem: flickable;
					}

					Flickable {
						id: flickable;
						anchors.centerIn: background;
						width: background.width - 2 * Style.marginM;
						height: background.height - 2 * Style.marginM;
						contentHeight: descriptionTextField.height;
						clip: true;
						boundsBehavior: Flickable.StopAtBounds;

						Text {
							id: descriptionTextField;
							width: parent.width;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;
							color: Style.textColor;
							wrapMode: Text.WordWrap;
						}
					}
				}
			}

			Loading {
				id: loading;
				anchors.fill: parent;
				visible: false
				background.color: Style.backgroundColor2
			}
		}
	}
}

