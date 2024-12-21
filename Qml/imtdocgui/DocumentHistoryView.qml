import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtbaseDocumentRevisionSdl 1.0

Rectangle {
	id: container;

	// color: Style.backgroundColor2;

	property int elementWidth: 450;
	property string documentId: "";
	property int activeRevision: 0;

	clip: true;

	onDocumentIdChanged: {
		if (documentId !== ""){
			request.send();
		}
	}

	GqlSdlRequestSender {
		id: request;
		gqlCommandId: ImtbaseDocumentRevisionSdlCommandIds.s_getRevisionInfoList;
		inputObjectComp: Component {
			GetRevisionInfoListInput {
				m_documentId: container.documentId;
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
	}

	Component {
		id: delegateComp;

		Column {
			width: column.width;

			Item {
				width: parent.width;
				height: elementView.height + 2*Style.size_mainMargin;

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
							font.pixelSize: Style.fontSize_XSmall;
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
						anchors.leftMargin: Style.size_mainMargin;
						spacing: Style.size_mainMargin;

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
		id: flickable;
		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.top: parent.top;
		anchors.topMargin: Style.size_mainMargin;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.size_mainMargin;
		width: container.elementWidth;

		contentWidth: width;
		contentHeight: column.height;

		boundsBehavior: Flickable.StopAtBounds;

		clip: true;

		Column {
			id: column;

			anchors.left: parent.left;
			anchors.leftMargin: Style.size_mainMargin;
			anchors.right: parent.right;
			anchors.rightMargin: Style.size_mainMargin;

			spacing: Style.size_mainMargin;

			Repeater {
				id: repeaterColumn;

				delegate: delegateComp;
			}
		}//Column main
	}

	CustomScrollbar {
		id: scrollbar;
		anchors.right: parent.right;
		anchors.rightMargin: Style.size_smallMargin;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		secondSize: Style.size_mainMargin;
		targetItem: flickable;
		radius: 2;
	}

	Text {
		anchors.centerIn: parent;
		font.pixelSize: Style.fontSize_common;
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
		color: Style.backgroundColor2;
	}
}

