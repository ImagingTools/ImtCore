import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtbaseDocumentRevisionSdl 1.0

Rectangle {
	id: container;

	color: Style.baseColor;

	property int elementWidth: 450;
	property alias contentHeight: column.height;
	property alias flickable: flickable_;
	property string documentId: "";
	property int activeRevision: 0;

	property string collectionId: "";

	property bool ok: collectionId !== "" && documentId !== "";

	onOkChanged: {
		if (ok){
			sendRequest();
		}
	}

	clip: true;

	onCollectionIdChanged: {
		if (collectionId !== ""){
			subscriptionClient.gqlCommandId = "On" + container.collectionId + "CollectionChanged";
		}
	}

	SubscriptionClient {
		id: subscriptionClient;
		onMessageReceived: {
			container.sendRequest();
		}
	}

	function sendRequest(){
		if (PermissionsController.checkPermission("ViewRevisions")){
			request.send();
		}
	}
	
	function getHeaders(){
		return {};
	}

	GqlSdlRequestSender {
		id: request;
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_getRevisionInfoList;
		inputObjectComp: Component {
			GetRevisionInfoListInput {
				m_documentId: container.documentId;
				m_collectionId: container.collectionId;
			}
		}

		sdlObjectComp: Component { RevisionInfoList {
				onFinished: {
					container.activeRevision = m_activeRevision;
					repeaterColumn.model = m_revisions;
				}
			}
		}

		onStateChanged: {
			loading.visible = (state === "Loading");
		}
		
		function getHeaders(){
			return container.getHeaders();
		}
	}

	Component {
		id: delegateComp;

		Column {
			width: column.width;
			visible: model.item.m_description !== "" && model.item.m_user !== "";

			Item {
				width: parent.width;
				height: elementView.height + 2*Style.marginM;

				ElementView {
					id: elementView;
					anchors.verticalCenter: parent.verticalCenter;
					width: parent.width;
					name: model.item.m_user + ", " + model.item.m_timestamp;

					clip: false;
					bottomComp: Component {
						Text {
							width: parent.width;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;
							color: Style.textColor;
							wrapMode: Text.WrapAnywhere;
							text: model.item.m_description;
						}
					}

					Row {
						id: row;
						z: 999;
						anchors.verticalCenter: parent.top;
						anchors.left: parent.left;
						anchors.leftMargin: Style.marginM;
						spacing: Style.marginM;

						StickerView {
							anchors.verticalCenter: parent.verticalCenter;
							color: Style.iconColorOnSelected;
							text: qsTr("Revision") + ": " + model.item.m_revision;
						}

						StickerView {
							anchors.verticalCenter: parent.verticalCenter;
							color: Style.errorColor;
							text: qsTr("Active");
							visible: model.item.m_revision == container.activeRevision;
						}
					}
				}
			}
		}
	}

	Flickable {
		id: flickable_;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.topMargin: Style.marginM;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginM;

		contentWidth: width;
		contentHeight: column.height;

		boundsBehavior: Flickable.StopAtBounds;
		interactive: false;

		clip: true;

		Column {
			id: column;

			anchors.left: parent.left;
			anchors.leftMargin: Style.marginM;
			anchors.right: parent.right;
			anchors.rightMargin: Style.marginM;

			spacing: Style.marginM;

			Repeater {
				id: repeaterColumn;

				delegate: delegateComp;
			}
		}//Column main
	}

	Text {
		anchors.centerIn: parent;
		font.pixelSize: Style.fontSizeM;
		font.family: Style.fontFamilyBold;
		font.bold: true;
		color: Style.textColor;
		wrapMode: Text.WrapAnywhere;
		text: qsTr("There is no history for this document");
		visible: repeaterColumn.count === 0;
	}

	Loading {
		id: loading;
		anchors.fill: parent;
		visible: false;
		background.color: Style.baseColor;
	}
}

