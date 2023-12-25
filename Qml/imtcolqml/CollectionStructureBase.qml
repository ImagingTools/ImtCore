import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: collectionStructureBaseContainer;

    property alias commands: gqlModels;

    property string commandId;

    property alias tableElementsDelegate: treeViewInternal.delegate;
    // property alias tableHeadersDelegate: treeViewInternal.headerDelegate;
    // property alias elementsList: treeViewInternal.elementsList;

    // property alias tableMinWidth: treeViewInternal.minWidth;
    // property alias tableHeaderHeight: treeViewInternal.headerHeight;
    // property alias tableItemHeight: treeViewInternal.itemHeight;
    // property alias tableDecoratorPath: loaderTableDecorator.source;
    // property alias tableDecoratorComp: loaderTableDecorator.sourceComponent;
    // property alias tableCellDecorator: treeViewInternal.cellDecorator;
    // property alias tableWidthDecorator: treeViewInternal.widthDecorator;
    // property alias tableWidth: treeViewInternal.width;
    // property alias tableHeaders: treeViewInternal.headers;

    property var treeView: treeViewInternal;
    property bool hasFilter: true;

    // property bool hasSort: true;
    // property alias isMultiCheckable: treeViewInternal.isMultiCheckable;

    property alias filterMenu: filterMenuLocal.sourceComponent;
    property alias filterMenuItem: filterMenuLocal.item;
    property alias filterMenuVisible: filterMenuLocal.visible;
    property alias modelFilter: modelFilterObj;

    signal selectedItem(string id, string name);
    signal selectedIndexChanged(int index);

    signal elementsChanged();

    signal selectionChanged(var selection);
    signal filterDecoratorLoaded();

    Component.onCompleted: {
        treeViewInternal.focus = true;

        Events.subscribeEvent("OnLocalizationChanged", collectionStructureBaseContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", collectionStructureBaseContainer.onLocalizationChanged);
    }


    onCommandIdChanged: {
        gqlModels.commandId = commandId;

        gqlModels.updateModels();
    }

    Loader {
        id: filterMenuLocal;

        anchors.top: collectionStructureBaseContainer.top;
        anchors.topMargin: Style.margin;

        width: parent.width;

        visible: treeViewInternal ? false : false;//for web, do not remove!!!

        onVisibleChanged: {
            if (!filterMenuLocal.visible){
                treeViewInternal.forceActiveFocus();
            }
        }

        sourceComponent: Component {
            FilterMenu {
                decoratorSource: Style.filterPanelDecoratorPath;
            }
        }

        onLoaded: {
            filterMenuLocal.item.textFilterChanged.connect(collectionStructureBaseContainer.onTextFilterChanged);
            filterMenuLocal.item.closed.connect(collectionStructureBaseContainer.onFilterClosed);
        }
    }

    function onLocalizationChanged(language){
        console.log("CollectionViewBase onLocalizationChanged", commandId);
        gqlModels.updateModels();
    }

    function onFilterClosed(){
        filterMenuLocal.visible = false;
    }

    function onTextFilterChanged(index, text){
        console.log("onTextFilterChanged", text);
        modelFilterObj.SetData("TextFilter", text);
        gqlModels.updateModels();
    }

    function updateModels(){
        gqlModels.updateModels();
    }

    Rectangle {
        id: backgroundTreeView;

        anchors.top: filterMenuLocal.visible ? filterMenuLocal.bottom: parent.top;
        anchors.topMargin: filterMenuLocal.visible ? Style.margin : 0;
        anchors.left: parent.left;

        width: parent.width

        anchors.bottom: parent.top;

        color: Style.baseColor;

        radius: Style.size_mainCornerRadius !== undefined ? Style.size_mainCornerRadius: 0;

        Loader{
            id: loaderTableDecorator;

            property bool compl: false;
            onLoaded: {
                if(loaderTableDecorator.item){
                    loaderTableDecorator.compl = true;
                }
            }
        }



        Item {
            id: rightPanel;

            anchors.top: parent.top;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.bottom: parent.bottom;

            width: collectionStructureBaseContainer.hasFilter ? 35 : 10;

            visible: treeViewInternal.width > 0;

            Item {
                id: filterItem;

                anchors.top: parent.top;

                width: parent.width;
                height: treeViewInternal.headerHeight;

                AuxButton {
                    id: iconFilter;

                    anchors.centerIn: parent;

                    width: Style.buttonWidthMedium;
                    height: width;

                    visible: collectionStructureBaseContainer.hasFilter;
                    highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

                    iconWidth: Style.iconSizeSmall;
                    iconHeight: iconWidth;

                    iconSource: "../../../" + Style.getIconPath("Icons/Filter", Icon.State.On, Icon.Mode.Normal);

                    onClicked: {
                        filterMenuLocal.visible = !filterMenuLocal.visible;
                    }
                }
            }
        }

        TreeViewGql{
            id: treeViewInternal
            anchors.left: parent.left;
            anchors.right: rightPanel.left;
            anchors.top: parent.top;
        }


        TreeItemModel {
            id: modelFilterObj;

            Component.onCompleted: {
                modelFilterObj.SetUpdateEnabled(true)
                modelFilterObj.AddTreeModel("FilterIds");
                console.log("modelFilterObj onCompleted", modelFilterObj.toJSON())
            }
        }

        Loading {
            id: ldng;

            anchors.fill: treeViewInternal;
            anchors.topMargin: treeViewInternal.headerElementHeight;

            visible: false;
        }
    }



    CollectionStructureBaseGqlModels {
        id: gqlModels;

        commandId: collectionStructureBaseContainer.commandId;
        rootItem: collectionStructureBaseContainer;

        onItemsChanged: {
            treeViewInternal.elements = gqlModels.items;
        }

        onItemsInfoGqlStateChanged: {
            if (state === "Loading"){
                ldng.visible = true;
            }
            else{
                ldng.visible = false;
            }
        }

    }
}
