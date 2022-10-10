import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: popupMenuContainer;

    width: popupMenuContainer.itemWidth;
    height: popupMenuListView.height;

    property var model;
    property Item root;

    property int itemWidth: 200;
    property int itemHeight: 26;
    property int offset: 0;
    property int delegateRadius: 0;
    property int textSize: Style.fontSize_common;
    property int count: 15;
    property string commandId: "";
    property string filterName: "Name";
    property alias filterText: filterField.text;
    property bool endListStatus: false;
    property Component delegate: PopupMenuDelegate{onClicked: {finished(commandId, index)}};
    property var properties;
    property var gettedParams;

    /**
        Count of the visible item, if value = -1 then count unlimited
    */
    property int countVisibleItem: -1;

    signal finished(string commandId, int index);
    signal endList();
    signal textEdited();




    TreeItemModel{
        id: modelFilter
    }
    TreeItemModel{
        id: filterIdsModel
    }



    onFinished: {
        root.closeDialog();
    }

    onModelChanged: {
        popupMenuListView.model = model;
    }
    onPropertiesChanged: {
        for (var item = 0; item < properties.GetItemsCount(); item++){
            modelFilter.SetData(properties.GetData("Id", item),  properties.GetData("Value", item));
        }
        itemsModel.updateModel(0);
//        root.closeDialog();
    }

    onRootChanged: {
        /**
            Opacity of the background = 0
        */
        root.backgroundItem.opacity = 0;

        /**
            Close the dialog by clicking on the background
        */
        root.backgroundItem.backgroundAreaItem.clicked.connect(root.closeDialog);
    }
    Component.onCompleted: {
        var index = filterIdsModel.InsertNewItem();
        filterIdsModel.SetData("Id", "Name", index);
        modelFilter.AddTreeModel("FilterIds");
        modelFilter.SetData("FilterIds", filterIdsModel)
        modelFilter.AddTreeModel("Sort");
        itemsModel.updateModel(0);
    }

    CustomTextField {
        id: filterField;
        z: 100;
        width: parent.width;
        height: 30;
        radius: popupMenuContainer.delegateRadius;
        anchors.top: parent.top
        anchors.left: parent.left
        textSize: popupMenuContainer.textSize;
        onTextEdited: {
            comboBoxContainer.currentIndex = -1;
            offset = 0;
            modelFilter.SetData("TextFilter", filterText);
            itemsModel.updateModel(0);
        }
        onAccepted: {
            root.closeDialog();
            popupMenuContainer.close();
        }

    }

    Rectangle {
        id: itemBody;
        anchors.top: filterField.bottom
        anchors.left: parent.left

        width: popupMenuContainer.width;
        height: popupMenuListView.height;
        radius: popupMenuContainer.delegateRadius;

        color: Style.baseColor;

        border.width: 1;
        border.color: Style.alternateBaseColor;

        Rectangle{
            id: loadedRec
            anchors.fill: parent
            opacity: 0.5
            visible: false;

            Text {

                anchors.centerIn: parent;

                text: "Loaded..."
                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
            }
        }

        ListView {
            id: popupMenuListView;
            width: popupMenuContainer.width;
            height: (countVisibleItem == -1 || countVisibleItem > popupMenuListView.count) ?
                        popupMenuListView.count * popupMenuContainer.itemHeight :
                        countVisibleItem * popupMenuContainer.itemHeight;

            boundsBehavior: Flickable.StopAtBounds;
            clip: true;
            onContentYChanged: {
                if(contentHeight - contentY - popupMenuListView.height == 0){
                    if(!endListStatus && itemsModel.state == "Ready"){
                        console.log("Create additional query")
                        loadedRec.visible = true;
                        offset = offset + popupMenuContainer.count;
                        console.log("Offset = ", offset);
                        itemsModel.updateModel(offset);
                    }
                }
            }
            //delegate: PopupMenuDelegate{textSize: popupMenuContainer.textSize}// Delegate Item
            delegate: popupMenuContainer.delegate;

        }//ListView
    }//ItemListView

    DropShadow {
       id: dropShadow;

       anchors.fill: itemBody;

       horizontalOffset: 2;
       verticalOffset: 2;

       radius: 4;
       color: Style.shadowColor;

       source: itemBody;
    }
    GqlModel {
        id: itemsModel;

        function updateModel(offsetVar) {
            var query = Gql.GqlRequest("query", commandId);

            var inputParams = Gql.GqlObject("input");
            query.AddParam(inputParams);
            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offsetVar);
            viewParams.InsertField("Count", count);
            viewParams.InsertField("FilterModel");
            var jsonString = modelFilter.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            viewParams.InsertField("FilterModel", jsonString);
            inputParams.InsertFieldObject(viewParams);
            var queryFields = Gql.GqlObject("items");
            for (var item = 0; item < gettedParams.GetItemsCount(); item++){
                var nameParam = gettedParams.GetData("Name", item);
                queryFields.InsertField(nameParam);
            }
            query.AddField(queryFields)
            var gqlData = query.GetQuery();

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){
                let dataModelLocal;

                if (itemsModel.ContainsKey("errors")){

                    return;
                }

                if (itemsModel.ContainsKey("data")){
                    dataModelLocal = itemsModel.GetData("data");

                    if (dataModelLocal.ContainsKey(commandId)){
                        dataModelLocal = dataModelLocal.GetData(commandId);
                        dataModelLocal = dataModelLocal.GetData("items");
                        console.log(commandId, " = ", dataModelLocal);
                        if (offset == 0){
                            model = dataModelLocal;
                            loadedRec.visible = false;
                        }
                        else{
                            console.log("count items = ", dataModelLocal.GetItemsCount())
                            if(dataModelLocal.GetItemsCount() > 0){
                                for (var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                                    model.InsertNewItem()
                                    dataModelLocal.CopyItemDataToModel(i, model, offset+i)
                                }
                            }
                            else{
                                endListStatus = true;
                            }
                            loadedRec.visible = false;
                        }
                    }
                    dataModelLocal.Refresh()
                }
            }
        }
    }
}
