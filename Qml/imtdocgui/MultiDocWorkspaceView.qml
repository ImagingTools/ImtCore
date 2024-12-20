import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: workspaceView;

    property DocumentManager documentManager;
    property alias tabPanel: tabPanel_;

	property int popupWidth: 200;

    onDocumentManagerChanged: {
        if (documentManager){
            connections.target = workspaceView.documentManager;
            tabPanel_.model = workspaceView.documentManager.documentsModel;
            documentRepeater.model = workspaceView.documentManager.documentsModel;
        }
        else{
            connections.target = undefined;
            tabPanel_.model = 0;
            documentRepeater.model = 0;
        }
    }

    Connections {
        id: connections;

        function onDocumentAdded(documentIndex, documentId){
            workspaceView.tabPanel.selectedIndex = documentIndex;
        }

        function onDocumentClosed(documentIndex, documentId){
            if (documentIndex <= workspaceView.tabPanel.selectedIndex && documentIndex > 0){
                workspaceView.tabPanel.selectedIndex--;
            }
        }
    }

    function addFixedView(viewComp, name, index){
        let viewName = name;
        if (!viewName || viewName == ""){
            viewName = workspaceView.documentManager.defaultDocumentName;
        }

        if (!index){
            index = workspaceView.documentManager.documentsModel.count;
        }

		workspaceView.documentManager.documentsModel.insert(index, {
                                  "Uuid": UuidGenerator.generateUUID(),
                                  "Title": viewName,
								  "ViewComp": viewComp,
                                  "Fixed": true
                              })

        workspaceView.documentManager.documentAdded(index, "");
    }

    function setAlertPanel(alertPanelComp){
        alertPanel.sourceComponent = alertPanelComp;
    }

    Rectangle {
        anchors.fill: parent;
        color: Style.backgroundColor2;
    }

	Component {
		id: popupMenuDialog;

		PopupMenuDialog {
			itemWidth: workspaceView.popupWidth;
			onFinished: {
				if (commandId === "Close"){
					if (tabPanel_.selectedIndex > 0){
						workspaceView.documentManager.closeDocumentByIndex(tabPanel_.selectedIndex);
					}
				}
				else if (commandId === "CloseAll"){
					let documentIds = workspaceView.documentManager.getOpenedDocumentIds();

					for (let i = 0; i < documentIds.length; i++){
						workspaceView.documentManager.closeDocument(documentIds[i], true);
					}
				}
			}
		}
	}

	LocalizationEvent {
		onLocalizationChanged: {
			tabContextMenuModel.fillModel();
		}
	}

	PopupMenuModel {
		id: tabContextMenuModel;
		Component.onCompleted: {
			fillModel();
		}

		function fillModel(){
			tabContextMenuModel.clear();
			tabContextMenuModel.addItem("Close", qsTr("Close document"), "", true);
			tabContextMenuModel.addItem("CloseAll", qsTr("Close all documents"), "", true);
		}
	}

    TabPanel {
        id: tabPanel_;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        clip: true;

		onTabClicked: {
			if (mouse.button === Qt.RightButton){
				let isFixedView = workspaceView.documentManager.documentsModel.get(index).Fixed;
				if (isFixedView !== undefined && isFixedView){
					return;
				}

				var point = tabItem.mapToItem(this, 0, 0);

				ModalDialogManager.openDialog(popupMenuDialog, {"x": point.x + workspaceView.popupWidth, "y": point.y, "model": tabContextMenuModel});
			}
		}

        onCloseItem: {
            workspaceView.documentManager.closeDocumentByIndex(index);
        }

        onRightClicked: {
            if (tabPanel_.selectedIndex < workspaceView.documentManager.documentsModel.count - 1){
                tabPanel_.selectedIndex++;
            }
        }

        onLeftClicked: {
            if (tabPanel_.selectedIndex > 0){
                tabPanel_.selectedIndex--;
            }
        }
    }

    Loader {
        id: alertPanel;
        anchors.top: tabPanel_.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        height: visible ? 40: 0;
        visible: alertPanel.item != null && alertPanel.item !== undefined;
    }

    Repeater {
        id: documentRepeater;
        anchors.top: alertPanel.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        clip: true;
        delegate: Item {
            anchors.fill: documentRepeater;
			visible: tabPanel_.selectedIndex === model.index;
			clip: true;
			Component.onCompleted: {
				if (model.Fixed !== undefined && model.Fixed){
					let item = model.ViewComp.createObject(this);
					item.anchors.fill = this;
					return;
				}

				let documentData = workspaceView.documentManager.getDocumentData(model.index);
				if (documentData && documentData.views.length > 0){
					documentData.views[0].parent = this;
					documentData.views[0].anchors.fill = this;
					documentData.views[0].visible = true;
				}
			}
        }
    }
}
