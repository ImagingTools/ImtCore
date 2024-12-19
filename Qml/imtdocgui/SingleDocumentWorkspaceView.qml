import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: root;

    color: Style.backgroundColor2;

    property string titleSeparation: "/";
    property bool initialItemTitleVisible: true;

    property DocumentManager documentManager;

    onDocumentManagerChanged: {
        if (documentManager){
            if (Qt.platform.os === "web"){
                documentManager.generateDocumentTitle = root.generateDocumentTitle;
            }

            connections.target = documentManager;
            headersListView.model = documentManager.documentsModel;
        }
        else{
            connections.target = undefined;
            headersListView.model = 0;
        }
    }

    Connections {
        id: connections;
        function onDocumentAdded(documentIndex, documentId){
			let documentData = root.documentManager.getDocumentData(documentIndex);
			if (documentData && documentData.views.length > 0){
				viewContainer.addTopView(documentData.views[0]);
			}
        }

        function onDocumentClosed(documentIndex, documentId){
			viewContainer.showLastChild();
        }
    }

    function generateDocumentTitle(documentIndex){
        if (documentIndex < 0 || documentIndex >= documentManager.documentsModel.count){
            console.warn("Unable generate document title. Document index is incorrect");

            return "";
        }

        let title = documentManager.defaultDocumentName;

        let documentData = documentManager.documentsModel.get(documentIndex).DocumentData;

        let documentName = "";

        if (documentData){
            documentName = documentData.getDocumentName();
        }

        if (documentName && documentName !== ""){
            title = documentName;
        }

        if (documentData && documentData.isDirty){
            title = title + " *";
        }

        return title;
    }

    function addInitialItem(viewComp, name){
        if (!documentManager){
            console.error("Unable to add the initial item to single document view. Error: Document manager is invalid");
            return;
        }

        documentManager.documentsModel.append({
                                                  "Uuid": UuidGenerator.generateUUID(),
                                                  "Title": name,
                                                  "DocumentViewComp": viewComp,
                                                  "Fixed": true
                                              });

		viewContainer.addTopView(viewComp.createObject(root))
    }

    function setAlertPanel(alertPanelComp){
        alertPanel.sourceComponent = alertPanelComp;
    }

    Loader {
        id: alertPanel;
        height: visible ? 40: 0;
        visible: false;
    }

    Item {
        id: buttonPanel;
        anchors.top: alertPanel.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: visible ? Style.size_mainMargin : 0;
        width: visible ? closeButton.width: 0;
        height: headersListView.height;
		visible: root.documentManager.documentsModel.count > 1;

        ToolButton {
            id: closeButton;
            anchors.centerIn: buttonPanel;
            width: visible ? 25: 0;
            height: width;
            iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal);

            onClicked: {
				root.documentManager.closeDocumentByIndex(root.documentManager.documentsModel.count - 1);
            }
        }
    }

    ListView {
        id: headersListView;
        anchors.top: alertPanel.bottom;
        anchors.left: buttonPanel.right;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: parent.right;
        height: visible ? 40 : 0;
        orientation: ListView.Horizontal;
        boundsBehavior: Flickable.StopAtBounds;
        visible: count === 1 && !root.initialItemTitleVisible ? false : true;
        delegate: Item {
            width: visible ? content.width + spacer.width: 0;
            height: headersListView.height;
            visible: model.index === 0 ? root.initialItemTitleVisible : true;

            Item {
                id: spacer;
                anchors.left: parent.left;
                width: visible ? Style.size_mainMargin : 0;
                visible: model.index === 1 && !root.initialItemTitleVisible ? false : true;
            }

            Row {
                id: content;
                anchors.left: spacer.right;
                height: parent.height;
                spacing: Style.size_mainMargin;

                Text {
                    id: separator;
                    anchors.verticalCenter: content.verticalCenter;
                    font.pixelSize: Style.fontSize_title;
                    font.family: Style.fontFamily;
                    color: Style.titleColor;
                    text: root.titleSeparation;
                    visible: model.index === 1 && !root.initialItemTitleVisible ? false : model.index > 0;
                }

                Text {
                    id: headerText;
                    anchors.verticalCenter: content.verticalCenter;
                    font.pixelSize: Style.fontSize_title;
                    font.family: Style.fontFamily;
                    color: Style.titleColor;
                    text: model.Title;
                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {}
            }
        }
    }

	Item {
		id: viewContainer;
		z: 10;
		anchors.top: headersListView.bottom;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;

		function showLastChild(){
			for (let i = 0; i < viewContainer.children.length - 1; i++){
				viewContainer.children[i].visible = false;
			}

			viewContainer.children[viewContainer.children.length - 1].visible = true;
		}

		function addTopView(view){
			view.parent = viewContainer;
			view.anchors.fill  = viewContainer;

			showLastChild();
		}
	}
}


