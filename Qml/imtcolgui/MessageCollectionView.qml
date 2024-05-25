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
            collectionHeadersModel.Clear();

            let index = collectionHeadersModel.InsertNewItem();
            collectionHeadersModel.SetData("Id", "Text", index);
            collectionHeadersModel.SetData("Name", qsTr("Description"), index);

            index = collectionHeadersModel.InsertNewItem();
            collectionHeadersModel.SetData("Id", "LastModified", index);
            collectionHeadersModel.SetData("Name", qsTr("Time"), index);

            index = collectionHeadersModel.InsertNewItem();
            collectionHeadersModel.SetData("Id", "Source", index);
            collectionHeadersModel.SetData("Name", qsTr("Source"), index);

            log.dataController.headersModel  = collectionHeadersModel;
        }

        onHeadersChanged: {
            if (log.table.headers.GetItemsCount() > 0){
                log.table.tableDecorator = logTableDecoratorModel
                log.table.setColumnContentComponent(0, messageColumnContentComp);
            }
        }


        Component.onCompleted: {
            console.log("DEBUG:log Component.onCompleted", collectionId, log.clientId)
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

            function getAdditionalInputParams(){
                console.log("LogCollectionView", log.clientId)
                return log.getAdditionalInputParams()
            }

            function updateModel(){
                console.log("Collection representation updateModel");
                log.doUpdateGui()
            }
        } }


        TreeItemModel {
            id: logTableDecoratorModel;

            Component.onCompleted: {
                var cellWidthModel = logTableDecoratorModel.AddTreeModel("CellWidth");

                let index = cellWidthModel.InsertNewItem();
                cellWidthModel.SetData("Width", -1, index);

                index = cellWidthModel.InsertNewItem();
                cellWidthModel.SetData("Width", 200, index);

                index = cellWidthModel.InsertNewItem();
                cellWidthModel.SetData("Width", 300, index);
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
                        let category = rowDelegate.tableItem.elements.GetData("Category", rowIndex);
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
                        modalDialogManager.openDialog(messageDialogComp, parameters);
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
