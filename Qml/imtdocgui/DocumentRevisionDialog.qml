import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

Dialog {
	id: documentRevisionDialog;
	title: qsTr("Revisions (%1)").arg(revisionsModel ? revisionsModel.count : 0);
	canMove: false;
	backgroundColor: Style.baseColor;

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
					PopupManager.addSuccessMessage(qsTr("The document revision has been successfully delete"));
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
					PopupManager.addSuccessMessage(qsTr("The document revision has been successfully set"))
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
			width: Style.sizeHintXXXL;
			height: Style.sizeHintXXL;

			property int visualSelectedRevision: -1;

			function selectRevision(revision){
				documentRevisionDialog.buttons.setButtonState(Enums.save, false);
				documentRevisionDialog.buttons.setButtonState(Enums.no, false);

				contentItem.visualSelectedRevision = revision;

				if (revision !== documentRevisionDialog.currentRevision){
					documentRevisionDialog.selectedRevision = revision;
					documentRevisionDialog.buttons.setButtonState(Enums.save, true);
					documentRevisionDialog.buttons.setButtonState(Enums.no, true);
				}
			}

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

					contentItem.visualSelectedRevision = -1;
					documentRevisionDialog.buttons.setButtonState(Enums.save, false);
					documentRevisionDialog.buttons.setButtonState(Enums.no, false);
				}
			}

			Rectangle {
				anchors.fill: parent;
				color: Style.baseColor;
			}

			Component {
				id: revisionCardComp;

				RevisionCard {
					width: revisionColumn.width;
					revision: model.item.m_revision;
					userName: model.item.m_user;
					timestamp: model.item.m_timestamp;
					description: model.item.m_description;
					isActive: model.item.m_isActive;
					selectable: true;
					selected: contentItem.visualSelectedRevision === model.item.m_revision;

					onClicked: contentItem.selectRevision(model.item.m_revision);
				}
			}

			Flickable {
				id: flickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginL;

				contentWidth: width;
				contentHeight: revisionColumn.height;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: revisionColumn;
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.rightMargin: revisionScrollbar.secondSize + Style.spacingS;
					spacing: Style.marginM;

					Repeater {
						id: repeaterColumn;
						model: documentRevisionDialog.revisionsModel;
						delegate: revisionCardComp;
					}
				}
			}

			CustomScrollbar {
				id: revisionScrollbar;
				z: flickable.z + 1;
				anchors.right: flickable.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
				alwaysVisible: false;
			}

			Column {
				anchors.centerIn: flickable;
				spacing: Style.marginM;
				visible: repeaterColumn.count === 0 && !loading.visible;

				Image {
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Style.iconSizeXL;
					height: width;
					source: "qrc:/" + Style.getIconPath("Icons/History", Icon.State.On, Icon.Mode.Normal);
					sourceSize.width: width;
					sourceSize.height: height;
					opacity: Style.opacityLow;
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter;
					font.pixelSize: Style.fontSizeL;
					font.family: Style.fontFamilyBold;
					color: Style.textColor;
					text: qsTr("No revisions yet");
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter;
					font.pixelSize: Style.fontSizeM;
					font.family: Style.fontFamily;
					color: Style.inactiveTextColor;
					text: qsTr("Saved revisions of this document will appear here");
				}
			}

			Loading {
				id: loading;
				anchors.fill: parent;
				visible: false;
				background.color: Style.baseColor;
			}
		}
	}
}

