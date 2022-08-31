import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: packageViewContainer;

    commandsDelegatePath: "../../imtlicgui/PackageViewCommandsDelegate.qml"

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

        collectionView.table.headers = headers;
        collectionView.table.elements = items;

        undoRedoManager.model = documentModel;
    }

    onCommandsDelegateLoaded: {
        commandsDelegate.tableData = collectionView.table;
    }

    onWidthChanged: {
        console.log("packageViewContainer onWidthChanged", packageViewContainer.width);
        if (packageViewContainer.width > 0 && packageViewContainer.width - rightPanel.width > 250){
            splitter.x = packageViewContainer.width - rightPanel.width;
        }
    }

    onCommandsIdChanged: {
        console.log("ObjectView onCommandsIdChanged", packageViewContainer.commandsId);
        collectionView.commandsId = commandsId;
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: packageViewContainer.commandsId;
        editorItem: packageViewContainer;

        onModelParsed: {

            collectionView.table.elements = documentModel.GetData("Items");
            updateGui();
        }
    }

    function updateGui(){
        console.log("PackageView updateGui")

        let index = collectionView.table.selectedIndex;
        collectionView.table.selectedIndex = -1;
        collectionView.table.selectedIndex = index;

        documentModel.Refresh();

        collectionView.table.elements.Refresh();
    }

    CollectionViewBase {
        id: collectionView;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        hasPagination: false;
        hasFilter: false;
        hasSort: false;

        loadData: false;

        height: parent.height;

        onSelectedItem: {
            commandsDelegate.commandActivated("Edit");
        }
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
        }

        TreeView {
            id: treeView;

            height: packageViewContainer.height;
            width: parent.width;

            clip: true;

            modelItems: treeViewModel.modelTreeView;
            visible: itemId !== "" && collectionView.table.selectedIndex > -1;

            onCheckBoxChanged: {
                console.log("PackageView onCheckBoxChanged", state, parentId, childId);
                treeViewController.checkBoxChanged(state, parentId, childId);
            }
        }
    }
}
