import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    anchors.fill: parent;

    property Item rootItem;

    property string itemId;
    property string itemName;
    property string commandsId;
    property string commandsDelegatePath: "ProductViewCommandsDelegate.qml";

    property string treeViewControllerPath: "ProductViewTreeViewController.qml";

    property string rightPanelTitle: qsTr("Features");

    signal selectedItem(string id, string name);

    Component.onCompleted: {
        productModel.SetData("DependentModel", lisensesFeaturesModel.modelLicenseFeatures);
    }

    onItemIdChanged: {
        productModel.SetData("Id", itemId);
    }

    onItemNameChanged: {
        productModel.SetData("Name", itemName);
    }

    onVisibleChanged: {
        if (container.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": commandsProvider.commandsModel,
                                                      "CommandsId": commandsProvider.commandsId});
            updateGui();
        }
    }

    onWidthChanged: {
        console.log("container onWidthChanged", container.width);
       if (container.width > 0 && container.width - rightPanel.width > 250){
           splitter.x = container.width - rightPanel.width;
       }
    }

    onCommandsIdChanged: {
        console.log("ObjectView onCommandsIdChanged", container.commandsId);
        commandsProvider.commandsId = container.commandsId;

        collectionView.commands.itemId = container.itemId;
        collectionView.commands.gqlModelHeadersInfo = container.commandsId + "Info";
        collectionView.commands.gqlModelItemsInfo = container.commandsId + "List";
    }

    TreeItemModel {
        id: productModel;
    }

    function updateGui(){
        let index = collectionView.table.selectedIndex;
        collectionView.table.selectedIndex = -1;
        collectionView.table.selectedIndex = index;

//        inputId.text = productModel.GetData("Id");
//        inputName.text = productModel.GetData("Name");

        collectionView.table.elements.Refresh();
    }

    CollectionViewBase {
        id: collectionView;

        anchors.left: parent.left;
        anchors.right: splitter.left;

        hasPagination: false;

        height: parent.height;

        onSelectedItem: {
            commandsDelegate.commandHandle("Edit");
        }

        onElementsChanged: {
            productModel.SetData("Items", collectionView.table.elements);
        }
    }

    CommandsProvider {
        id: commandsProvider;
    }

    ProductViewCommandsDelegate {
        id: commandsDelegate;
        tableData: collectionView.table;
        objectView: container;
        commandsProvider: commandsProvider;

        commandsId: container.commandsId;
        objectModel: productModel;
    }

    Splitter {
        id: splitter;
        x: container.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!container.visible){
                return;
            }

            if (splitter.x > container.width - titleHeader.width){
                splitter.x = container.width - splitter.width;
            }

            if (splitter.x < 250){
                splitter.x = 250;
            }
        }
    }

    Column {
        id: rightPanel;

        anchors.left: splitter.right;

        width: container.width > 0 ? container.width - collectionView.width : 250;
        height: parent.height;

        spacing: 5;

        Text {
            id: titleId;

            anchors.left: headerTreeView.left;
            anchors.leftMargin: 10;

            text: qsTr("Product-ID");
            color: Style.textColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }

        CustomTextField {
            id: inputId;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the Product-ID");

            text: itemId;

            onTextChanged: {
                productModel.SetData("Id", inputId.text);
            }
        }

        Text {
            id: titleName;

            anchors.left: headerTreeView.left;
            anchors.leftMargin: 10;

            text: qsTr("Product Name");
            color: Style.textColor;
            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;
        }

        CustomTextField {
            id: inputName;

            width: parent.width;
            height: 30;

            text: itemName;

            placeHolderText: qsTr("Enter the Product Name");

            onTextChanged: {
                productModel.SetData("Name", inputName.text);
            }
        }

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

                text: container.rightPanelTitle;
                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;
            }
        }

        ProductTreeViewController {
            id: treeViewController;
        }

        TreeView {
            id: treeView;

            height: container.height;
            width: parent.width;

            clip: true;

            modelItems: treeViewModel.modelTreeView;
            visible: itemId !== "" && collectionView.table.selectedIndex > -1;

            onCheckBoxChanged: {
                treeViewController.checkBoxChanged(state, parentId, childId);
            }
        }
    }
}
