import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0


RemoteCollectionView {
    id: log;

    width: parent.width
    height: 200;

    commandsControllerComp: null
    table.enableAlternating: false

    TreeItemModel {
        id: collectionHeadersModel;

        Component.onCompleted: {
            log.updateHeaders();
        }
    }

    function updateHeaders(){
        collectionHeadersModel.clear();

        let index = collectionHeadersModel.insertNewItem();
        collectionHeadersModel.setData("Id", "Text", index);
        collectionHeadersModel.setData("Name", qsTr("Description"), index);

        index = collectionHeadersModel.insertNewItem();
        collectionHeadersModel.setData("Id", "LastModified", index);
        collectionHeadersModel.setData("Name", qsTr("Time"), index);

        index = collectionHeadersModel.insertNewItem();
        collectionHeadersModel.setData("Id", "Source", index);
        collectionHeadersModel.setData("Name", qsTr("Source"), index);

        log.dataController.headersModel  = collectionHeadersModel;
    }

    onHeadersChanged: {
        if (log.table.headers.getItemsCount() > 0){
            log.table.tableDecorator = logTableDecoratorModel
            log.table.setColumnContentComponent(0, messageColumnContentComp);
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");
        filterMenu.decorator = messageCollectionFilterComp;
    }

    function onFilterChanged(filterId, filterValue) {
        if (filterId === "TextFilter"){
            collectionFilter.setTextFilter(filterValue);
        }
        else{
            collectionFilter.setMessageStatusFilter(filterId, filterValue);
        }

        log.doUpdateGui();
    }

    dataControllerComp: Component { CollectionRepresentation {
            id: messageCollectionRepresentation
            additionalFieldIds: ["Id", "Name", "Category"]

            function getHeaders(){
                return log.getHeaders()
            }

            function updateModel(){
                console.log("Collection representation updateModel");
                log.doUpdateGui()
            }
        } }


    TreeItemModel {
        id: logTableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = logTableDecoratorModel.addTreeModel("CellWidth");

            let index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", 200, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", 300, index);
        }
    }

    Component {
        id: messageCollectionFilterComp;

        MessageCollectionFilterDecorator {}
    }

    Component {
        id: messageColumnContentComp;
        TableCellIconTextDelegate {
            id: cellDelegate
            onRowIndexChanged: {
                if (rowIndex >= 0){
                    let category = rowDelegate.tableItem.elements.getData("Category", rowIndex);
                    if (category === 0){
                        icon.source = "../../../../" + Style.getIconPath("Icons/Diagnostics", Icon.State.On, Icon.Mode.Normal);
                    }
                    else if (category === 1){
                        icon.source = "../../../../" + Style.getIconPath("Icons/Info", Icon.State.On, Icon.Mode.Normal);
                    }
                    else if (category === 2){
                        icon.source = "../../../../" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal);
                    }
                    else if (category === 3){
                        icon.source = "../../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);
                    }
                    else if (category === 4){
                        icon.source = "../../../../" + Style.getIconPath("Icons/Critical", Icon.State.On, Icon.Mode.Normal);
                    }
                }
            }

            MouseArea {
                id: ma;

                anchors.fill: parent
                propagateComposedEvents: true;
                onDoubleClicked: {
                    var parameters = {"centered": true, "message": tooltipArea.text};
                    ModalDialogManager.openDialog(messageDialogComp, parameters);
                }
            }

            TooltipArea {
                id: tooltipArea;

                mouseArea: ma;

                text: cellDelegate.getValue()
            }
        }
    }

    Component {
        id: messageDialogComp;
        MessageDialog {
            title: "Message";

            Component.onCompleted: {
                buttonsModel.clear()
                buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
            }
        }
    }
}
