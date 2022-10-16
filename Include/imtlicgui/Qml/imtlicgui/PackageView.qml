import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: packageViewContainer;

//    commandsDelegate: PackageViewCommandsDelegate {}
    commandsDelegatePath: "../../imtlicgui/PackageViewCommandsDelegate.qml";

    Component.onCompleted: {
        commandsDelegate.tableTreeViewEditor = collectionView;
    }

    onDocumentModelChanged: {
        console.log("documentBase onDocumentModelChanged");

        let headers = documentModel.GetData("Headers");
        let items = documentModel.GetData("Items");
        let dependencies = documentModel.GetData("Dependencies");

        if (!items){
            items = documentModel.AddTreeModel("Items");
        }

        if (!dependencies){
            dependencies = documentModel.AddTreeModel("Dependencies");
        }

        collectionView.headers = headers;
        collectionView.elements = items;

        for (let i = 0; i < treeView.model.GetItemsCount(); i++){
            let packageId = treeView.model.GetData("Id", i);

            if (packageId == itemId){
                treeView.model.SetData("ChildModel", collectionView.elements, i);
                break;
            }
        }
    }

    onWidthChanged: {
        if (packageViewContainer.width > 0 && packageViewContainer.width - rightPanel.width > 250){
            splitter.x = packageViewContainer.width - rightPanel.width;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: packageViewContainer.commandsId;
        editorItem: packageViewContainer;

        onModelParsed: {
        }
    }

    function updateGui(){
        console.log("PackageView updateGui")

        collectionView.elements = 0;
        collectionView.elements = documentModel.GetData("Items");
    }

    TableTreeViewEditor {
        id: collectionView;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        height: parent.height;
    }

    Splitter {
        id: splitter;
        x: packageViewContainer.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!packageViewContainer.visible){
                return;
            }

            if (splitter.x > packageViewContainer.width - titleHeader.width){
                splitter.x = packageViewContainer.width - splitter.width;
            }

            if (splitter.x < 250){
                splitter.x = 250;
            }
        }
    }

    Column {
        id: rightPanel;

        anchors.left: splitter.right;

        width: packageViewContainer.width > 0 ? packageViewContainer.width - collectionView.width : 250;
        height: parent.height;

        Rectangle {
            id: headerTreeView;

            width: parent.width;
            height: 35;

            color: Style.baseColor;

            Text {
                id: titleHeader;

                anchors.verticalCenter: headerTreeView.verticalCenter;

                anchors.left: headerTreeView.left;
                anchors.leftMargin: 10;

                text: qsTr("Dependencies");
                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;
            }
        }

        Rectangle{
            id: bottomLine;

            width: parent.width;
            height: 1;

            //TODO -> Style
            color: "lightgray";
        }

        PackageTreeViewController {
            id: treeViewController;

            documentItem: packageViewContainer;
        }

        TreeView {
            id: treeView;

            height: packageViewContainer.height;
            width: parent.width;

            clip: true;

//            modelItems: treeViewModel.modelTreeView;
            visible: itemId !== "" && collectionView.selectedIndex != null;
        }
    }
}
