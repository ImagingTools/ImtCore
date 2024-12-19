import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: workspaceView;

    property DocumentManager documentManager;
    property alias tabPanel: tabPanel_;

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
//            let viewComp = workspaceView.documentManager.documentsModel.get(documentIndex).DocumentViewComp;
//            let view = viewComp.createObject(viewsPanel);
//            view.anchors.fill = viewsPanel;
//            viewsPanel.updateVisibleItems();

//            let documentData = workspaceView.documentManager.documentsModel.get(documentIndex).DocumentData
//            if (documentData){
//                documentData.views.push(view);
//            }
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
                                  "DocumentViewComp": viewComp,
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

    TabPanel {
        id: tabPanel_;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        clip: true;

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
					let item = model.DocumentViewComp.createObject(this);
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

	  //       Loader {
	  //           id: documentLoader;
	  //           anchors.fill: parent;
	  //           sourceComponent: model.DocumentViewComp;
	  //           clip: true;
	  //           visible: tabPanel_.selectedIndex === model.index;

	  //           onLoaded: {
	  //               if (item.viewId !== undefined){
	  //                   item.viewId = model.Uuid;
	  //               }

	  //               if (model.Fixed !== undefined && model.Fixed){
	  //                   return;
	  //               }

	  //               let documentData = workspaceView.documentManager.documentsModel.get(model.index).DocumentData;
	  //               if (documentData){
						// documentData.addView(item);
	  //               }

	  //               workspaceView.documentManager.updateDocumentTitle(model.index);
	  //           }

	  //           onStatusChanged: {
	  //               if (status === Loader.Error){
	  //                   console.error("Document loading was failed");
	  //               }
	  //           }
	  //       }
        }
    }
}
