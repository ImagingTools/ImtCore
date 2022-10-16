import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: collectionViewBaseContainer;

    property alias commands: baseCommands;

    property string commandsId;

    property alias tableElementsDelegate: tableInternal.delegate;
    property alias tableHeadersDelegate: tableInternal.headerDelegate;
    property alias elementsList: tableInternal.elementsList;

    property var table: tableInternal;
    property bool hasPagination: true;
    property bool hasFilter: true;
    property bool hasSort: true;

    signal selectedItem(string id, string name);
    signal selectedIndexChanged(int index);
    signal elementsChanged();

    /**
        Если true -> данные будут запрошены с сервера,
        иначе нужно будет подставлять данные вручную
      */
    property bool loadData;

    Component.onCompleted: {
        console.log("CollectionViewBase onCompleted");

        tableInternal.focus = true;
    }

    onCommandsIdChanged: {
        console.log("CollectionViewBase onCommandsIdChanged", loadData);

        if (loadData){
            baseCommands.updateModels();
        }
    }

    Shortcut {
        sequence: "Ctrl+F";
        onActivated: {
            console.log("onActivated Ctrl+F");
            filterMenu.visible = !filterMenu.visible;
        }

        context: Qt.ApplicationShortcut;
    }

    FilterMenu {
        id: filterMenu;

        anchors.top: collectionViewBaseContainer.top;

        width: parent.width;

        decoratorSource: Style.filterPanelDecoratorPath;

        visible: false;

        onTextFilterChanged: {
            modelFilter.SetData("TextFilter", text);
            baseCommands.updateModels();
        }

        onClosed: {
            filterMenu.visible = false;
        }
    }

    Rectangle {
        id: backgroundTable;

        anchors.top: filterMenu.visible ? filterMenu.bottom: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        color: Style.baseColor;

        radius: thumbnailDecoratorContainer.mainRadius;

        AuxTable {
            id: tableInternal;

            anchors.fill: parent;
            anchors.bottom: pagination.visible ? pagination.top : parent.bottom;
            anchors.margins: thumbnailDecoratorContainer.mainMargin;

            hasFilter: collectionViewBaseContainer.hasFilter;
            hasSort: collectionViewBaseContainer.hasSort;

            onSelectItem: {
                collectionViewBaseContainer.selectedItem(idSelected, name);
            }

            onSelectedIndexChanged: {
                console.log("CollectionView AuxTable onSelectedIndexChanged");
                collectionViewBaseContainer.selectedIndexChanged(tableInternal.selectedIndex);
            }

            onTextFilterChanged: {
                modelFilter.SetData("TextFilter", text);
            }

            onElementsChanged: {
                collectionViewBaseContainer.elementsChanged();
            }

            onFilterClicked: {
                filterMenu.visible = !filterMenu.visible;
            }
        }
    }

    SortController {
        id: sortController;

        Component.onCompleted: {
            tableInternal.headerClicked.connect(sortController.headerClicked);
        }

        Component.onDestruction: {
            tableInternal.headerClicked.disconnect(sortController.headerClicked);
        }
    }

    function setHeaderSort(headerId, sortOrder){
        var filterLocal = modelFilter.GetData("Sort");
        filterLocal.SetData("HeaderId", headerId);
        filterLocal.SetData("SortOrder", sortOrder);
    }

    TreeItemModel {
        id: modelFilter;

        Component.onCompleted: {
            modelFilter.AddTreeModel("FilterIds");
            modelFilter.AddTreeModel("Sort");
        }
    }

    Pagination {
        id: pagination;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;

        pagesSize: 1;

        visible: collectionViewBaseContainer.hasPagination;

        onCurrentValueChanged: {
            console.log("Pagination onCurrentValueChanged", pagination.currentValue);
            tableInternal.selectedIndex = -1;
            baseCommands.updateModels();
        }
    }

    CollectionViewBaseGqlModels {
        id: baseCommands;

        commandsId: collectionViewBaseContainer.commandsId;

        onHeadersChanged: {
            tableInternal.headers = baseCommands.headers;
        }

        onItemsChanged: {
            tableInternal.elements = baseCommands.items;
        }
    }
}
