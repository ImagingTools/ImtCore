import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

CollectionViewCommandsDelegateBase {
	id: root;

	property DocumentManager documentManager: null;

	property string documentTypeId; // default document type-ID
	property string viewTypeId; // default document view type-ID

	onCollectionViewChanged: {
		if (collectionView && collectionView.dataController){
			collectionView.dataController.removed.connect(internal.onRemoved);
		}
	}

	QtObject {
		id: internal;

		function onRemoved(objectId){
			if (root.documentManager){
				let index = root.documentManager.getDocumentIndexByDocumentId(objectId);
				if (index >= 0){
					root.documentManager.closeDocument(objectId, true);
				}
			}
		}
	}

	function createNewObject(typeId, viewTypeId){
		if (root.documentManager){
			root.documentManager.insertNewDocument(typeId, viewTypeId);
		}
		else{
			console.error("Unable to create new object:", typeId, viewTypeId, ". Error: Document manager is invalid")
		}
	}

	function openDocumentEditor(objectId, typeId, viewTypeId){
		if (root.documentManager){
			root.documentManager.openDocument(objectId, typeId, viewTypeId);
		}
		else{
			console.error("Unable to open document for editing", typeId, viewTypeId, ". Error: Document manager is invalid");
		}
	}

	function onEdit(){
		let elementsModel = root.collectionView.table.elements;
		if (!elementsModel){
			console.error("Unable to edit document. Error: Elements for collection view is invalid");
			return;
		}

		let indexes = root.collectionView.table.getSelectedIndexes();
		for (let i = 0; i < indexes.length; i++){
			let index = indexes[i];
			if (elementsModel.containsKey("Id", index)){
				let itemId = elementsModel.getData("Id", index);

				root.openDocumentEditor(itemId, documentTypeId, viewTypeId);
			}
		}
	}

	function onRevision(){
		let elementsModel = root.collectionView.table.elements;
		if (!elementsModel){
			console.error("Unable to get revisions for document. Error: Elements for collection view is invalid");
			return;
		}

		let indexes = root.collectionView.table.getSelectedIndexes();
		if (indexes.length > 0){
			let index = indexes[0];
			if (elementsModel.containsKey("Id", index)){
				let documentId = elementsModel.getData("Id", index);
				ModalDialogManager.openDialog(documentRevisionDialogComp, {"documentId": documentId});
			}
		}
	}

	function onNew(){
		root.createNewObject(documentTypeId, viewTypeId);
	}

	Component {
		id: documentRevisionDialogComp;

		Dialog {
			id: documentRevisionDialog;
			title: qsTr("Revision Manager");
			canMove: false;

			notClosingButtons: Enums.no | Enums.save;

			property string documentId;
			property int selectedRevision: -1;
			property int currentRevision: -1;
			property BaseModel revisionsModel;

			Component.onCompleted: {
				if (PermissionsController.checkPermission("RestoreRevision")){
					buttonsModel.append({Id: Enums.save, Name:qsTr("Set this revision"), Enabled: false})
				}

				if (PermissionsController.checkPermission("DeleteRevision")){
					buttonsModel.append({Id: Enums.no, Name:qsTr("Delete revision"), Enabled: false})
				}

				buttonsModel.append({Id: Enums.cancel, Name:qsTr("Close"), Enabled: true})
			}

			onFinished: {
				if (buttonId == Enums.save){
					setRevisionRequest.send();
				}
				else if (buttonId == Enums.no){
					ModalDialogManager.openDialog(removeRevisionConfirmDialog, {});
				}
			}

			GqlSdlRequestSender {
				id: request;
				gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_getRevisionInfoList;
				inputObjectComp: Component {
					GetRevisionInfoListInput {
						m_documentId: documentRevisionDialog.documentId;
					}
				}

				sdlObjectComp: Component { RevisionInfoList {
						onFinished: {
							console.log("onFinished", m_revisions.toJson());
							documentRevisionDialog.revisionsModel = m_revisions;
							documentRevisionDialog.currentRevision = m_activeRevision;
						}
					}
				}

				onStateChanged: {
					// loading.visible = (state === "Loading");
				}
			}

			GqlSdlRequestSender {
				id: deleteRevisionRequest;
				gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_deleteRevision;
				inputObjectComp: Component {
					DeleteRevisionInput {
						m_objectId: documentRevisionDialog.documentId;
						m_revision: documentRevisionDialog.selectedRevision;
					}
				}

				sdlObjectComp: Component { DeleteRevisionResponse {
						onFinished: {
							request.send();
							ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully delete"));
						}
					}
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
					}
				}

				sdlObjectComp: Component { RestoreRevisionResponse {
						onFinished: {
							request.send();
							ModalDialogManager.showInfoDialog(qsTr("The document revision has been successfully set"));
						}
					}
				}
			}

			Component {
				id: removeRevisionConfirmDialog;
				MessageDialog {
					width: 400;
					title: qsTr("Remove revision");
					message: qsTr("Delete the selected revision?");
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
					width: 700;
					height: 600;

					Connections {
						target: documentRevisionDialog;

						function onStarted(){
							request.send();
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
								anchors.leftMargin: Style.size_mainMargin;
								width: 20;
								height: width;
								source: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
								sourceSize.width: width;
								sourceSize.height: height;
							}

							onRowIndexChanged: {
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

					SplitView {
						id: splitView;
						anchors.left: parent.left;
						anchors.leftMargin: Style.size_mainMargin;
						anchors.right: parent.right;
						anchors.rightMargin: Style.size_mainMargin;
						height: parent.height;
						orientation: Qt.Vertical

						Table {
							id: table;
							width: parent.width;
							height: 400;
							elements: documentRevisionDialog.revisionsModel;

							onElementsChanged: {
								table.resetSelection();
							}

							onHeadersChanged: {
								setColumnContentById("m_isActive", isActiveColumnDelegateComp);
							}

							onSelectionChanged: {
								documentRevisionDialog.buttons.setButtonState(Enums.save, false);
								documentRevisionDialog.buttons.setButtonState(Enums.no, false);

								let isEnabled = selection.length === 1;
								if (isEnabled){
									if (selection.length > 0){
										let index = selection[0];
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
								id: headersModel;
								Component.onCompleted: {
									let index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_user", index)
									headersModel.setData("Name", qsTr("User"), index)

									index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_revision", index)
									headersModel.setData("Name", qsTr("Revision"), index)

									index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_isActive", index)
									headersModel.setData("Name", qsTr("Active"), index)

									index = headersModel.insertNewItem();
									headersModel.setData("Id", "m_timestamp", index)
									headersModel.setData("Name", qsTr("Time"), index)

									table.headers = headersModel;
								}
							}
						}

						Item {
							id: descriptionItem;
							width: parent.width;
							height: 200;

							Text {
								id: descriptionTitle;
								anchors.top: parent.top;
								anchors.topMargin: Style.size_mainMargin;
								color: Style.buttonText;
								font.family: Style.fontFamilyBold;
								font.pixelSize: Style.fontSize_common;
								font.bold: true;
								text: qsTr("Description");
							}

							Rectangle {
								id: background;
								anchors.top: descriptionTitle.bottom;
								anchors.topMargin: Style.size_mainMargin;
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
								secondSize: Style.size_smallMargin;
								targetItem: flickable;
							}

							Flickable {
								id: flickable;
								anchors.centerIn: background;
								width: background.width - 2 * Style.size_mainMargin;
								height: background.height - 2 * Style.size_mainMargin;
								contentHeight: descriptionTextField.height;
								clip: true;
								boundsBehavior: Flickable.StopAtBounds;

								Text {
									id: descriptionTextField;
									width: parent.width;
									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSize_common;
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
					}
				}
			}
		}
	}
}
