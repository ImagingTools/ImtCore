import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: packageViewContainer;

    anchors.fill: parent;

    property string itemId;
    property string itemName;
    property string commandsId;

    property alias commands: commandsDelegate;

    signal selectedItem(string id, string name);

    Component.onCompleted: {
        console.log("PackageView onCompleted", featureDependenciesModel.modelFeatureDependencies);
        packageModel.SetExternTreeModel("DependentModel", featureDependenciesModel.modelFeatureDependencies);
    }

    onItemIdChanged: {
        console.log("packageViewContainer onItemIdChanged", itemId);
        packageModel.SetData("Id", itemId);
    }

    onItemNameChanged: {
        packageModel.SetData("Name", itemName);
    }

    onVisibleChanged: {
        if (packageViewContainer.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});

            updateGui();
        }
    }

    onWidthChanged: {
        console.log("packageViewContainer onWidthChanged", packageViewContainer.width);
        if (packageViewContainer.width > 0 && packageViewContainer.width - rightPanel.width > 250){
            splitter.x = packageViewContainer.width - rightPanel.width;
        }
    }

    onCommandsIdChanged: {
        console.log("ObjectView onCommandsIdChanged", packageViewContainer.commandsId);
        commandsProvider.commandsId = packageViewContainer.commandsId;

        collectionView.commands.itemId = packageViewContainer.itemId;
        collectionView.commands.gqlModelHeadersInfo = packageViewContainer.commandsId + "Info";
        collectionView.commands.gqlModelItemsInfo = packageViewContainer.commandsId + "List";
    }

    TreeItemModel {
        id: packageModel;

        onModelChanged: {
            if (!visible){
                return;
            }

            console.log("PackageView onModelChanged", itemId);
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: packageViewContainer.commandsId;
        editorItem: packageViewContainer;

        onModelParsed: {

            featureDependenciesModel.modelFeatureDependencies = packageModel.GetData("DependentModel");
            collectionView.table.elements = packageModel.GetData("Items");

            updateGui();
        }
    }

    function updateGui(){
        console.log("PackageView updateGui")

        let index = collectionView.table.selectedIndex;
        collectionView.table.selectedIndex = -1;
        collectionView.table.selectedIndex = index;

        collectionView.table.elements.Refresh();
    }

    CollectionViewBase {
        id: collectionView;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        hasPagination: false;
        hasFilter: false;
        hasSort: false;

        height: parent.height;

        onSelectedItem: {
            commandsDelegate.commandActivated("Edit");
        }

        onElementsChanged: {
            packageModel.SetExternTreeModel("Items", collectionView.table.elements);

            undoRedoManager.model = packageModel;
            commandsDelegate.objectModel = packageModel;
        }
    }

    CommandsProvider {
        id: commandsProvider;
    }

    PackageViewCommandsDelegate {
        id: commandsDelegate;

        tableData: collectionView.table;
        objectView: packageViewContainer;

        commandsId: packageViewContainer.commandsId;
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
