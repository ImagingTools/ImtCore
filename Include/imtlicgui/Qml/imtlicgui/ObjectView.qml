import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: objectViewContainer;

    anchors.fill: parent;

    property Item rootItem;

    property string itemId;
    property string itemName;
    property string commandsId;
    property string commandsDelegatePath;

    property string treeViewControllerPath;

    property string rightPanelTitle;

    signal selectedItem(string id, string name);

    onVisibleChanged: {
        if (objectViewContainer.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});
        }
    }

    onWidthChanged: {
        console.log("objectViewContainer onWidthChanged", objectViewContainer.width);
       if (objectViewContainer.width > 0 && objectViewContainer.width - rightPanel.width > 250){
           splitter.x = objectViewContainer.width - rightPanel.width;
       }
    }

    onCommandsIdChanged: {
        console.log("ObjectView onCommandsIdChanged", objectViewContainer.commandsId);
        commandsProvider.commandsId = objectViewContainer.commandsId;

        collectionView.commands.itemId = objectViewContainer.itemId;
        collectionView.commands.gqlModelHeadersInfo = objectViewContainer.commandsId + "Info";
        collectionView.commands.gqlModelItemsInfo = objectViewContainer.commandsId + "List";

        if (commandsLoader.item){
            commandsLoader.item.commandsId = objectViewContainer.commandsId;
        }
    }

    function updateGui(){
        collectionView.table.elements.Refresh();
    }

    CollectionViewBase {
        id: collectionView;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        hasPagination: false;

        height: parent.height;

        onSelectedItem: {
            if (commandsLoader.item){
                commandsLoader.item.commandHandle("Edit");
            }
        }
    }

    CommandsProvider {
        id: commandsProvider;
    }

    Loader {
        id: commandsLoader;

        Component.onCompleted: {
            console.log("commandsLoader onCompleted", objectViewContainer.commandsDelegatePath);
            commandsLoader.source = objectViewContainer.commandsDelegatePath;
        }

        onItemChanged: {
            if (commandsLoader.item){
                console.log("commandsLoader.item.tableItem ", collectionView.table);
                commandsLoader.item.tableData = collectionView.table;
                commandsLoader.item.objectView = objectViewContainer;
                commandsLoader.item.commandsProvider = commandsProvider;
            }
        }
    }

    Splitter {
        id: splitter;
        x: objectViewContainer.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!objectViewContainer.visible){
                return;
            }

            if (splitter.x > objectViewContainer.width - titleHeader.width){
                splitter.x = objectViewContainer.width - splitter.width;
            }

            if (splitter.x < 250){
                splitter.x = 250;
            }
        }
    }

    Column {
        id: rightPanel;

        anchors.left: splitter.right;

        width: objectViewContainer.width > 0 ? objectViewContainer.width - collectionView.width : 250;
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

                text: objectViewContainer.rightPanelTitle;
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

            color: "lightgray";
        }

        Loader {
            id: treeViewController;

            source: objectViewContainer.treeViewControllerPath;

            onLoaded: {
                treeViewController.treeViewItem = treeView;
                treeViewController.tableItem = collectionView.table;
            }
        }

        TreeView {
            id: treeView;

            height: objectViewContainer.height;
            width: parent.width;

            clip: true;

            modelItems: treeViewModel.modelTreeView;

            onCheckBoxStateChanged: {
                if (treeViewController.item){
                    treeViewController.item.checkBoxStateChanged(state, parentId, childId);
                }
            }
        }
    }
}
