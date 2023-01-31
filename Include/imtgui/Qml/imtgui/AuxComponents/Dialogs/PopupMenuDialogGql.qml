import QtQuick 2.12
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: popupMenuContainer;

    width: itemWidth;
    height: popupMenuListView.height;

    property var model;
    property Item root: null;
    property Item rootItem: null;

    property int itemWidth: 200;
    property int itemHeight: 26;
    property int filterHeight: 30;
    property int offset: 0;
    property int delegateRadius: 0;
    property int textSize: Style.fontSize_common;
    property int count: 15;
    property string commandId: "";
    property string filterName: "Name";
    property string fontColor: Style.textColor;
    property alias filterText: filterField.text;
    property bool endListStatus: false;
    property bool hiddenBackground: true;
    property bool canClose: true;

    property Component delegate: PopupMenuDelegate{
        textSize: popupMenuContainer.textSize;
        fontColor: popupMenuContainer.fontColor;
        onClicked: {popupMenuContainer.finished(commandId, index)}

        rootItem: popupMenuContainer;
    }


    property var properties;
    property var gettedParams;
    property var filterIdsModel;

    /**
        Count of the visible item, if value = -1 then count unlimited
    */
    property int countVisibleItem: -1;

    signal finished(string commandId, int index);
    signal endList();
    signal textEdited();

    TreeItemModel{
        id: modelFilter;
    }

    onFinished: {
        if(popupMenuContainer.canClose){
            popupMenuContainer.root.closeDialog();
        }
    }

    onModelChanged: {
        popupMenuListView.model = popupMenuContainer.model;
    }
    onPropertiesChanged: {
        for (var item = 0; item < properties.GetItemsCount(); item++){
            modelFilter.SetData(properties.GetData("Id", item),  popupMenuContainer.properties.GetData("Value", item));
        }
        itemsModel.updateModel(0);
//        root.closeDialog();
    }

    onRootChanged: {
        /**
            Opacity of the background = 0
        */
        if(popupMenuContainer.hiddenBackground){
            popupMenuContainer.root.backgroundItem.opacity = 0;
        }

        /**
            Close the dialog by clicking on the background
        */
        popupMenuContainer.root.backgroundItem.backgroundAreaItem.clicked.connect(popupMenuContainer.root.closeDialog);
    }
    Component.onCompleted: {
        modelFilter.AddTreeModel("FilterIds");
        modelFilter.SetData("FilterIds", filterIdsModel)
        modelFilter.AddTreeModel("Sort");
        itemsModel.updateModel(0);
    }

    Component.onDestruction: {
        popupMenuContainer.root.backgroundItem.backgroundAreaItem.clicked.disconnect(popupMenuContainer.root.closeDialog);
    }


    CustomTextField {
        id: filterField;

        z: 100;
        width: parent.width;
        height: popupMenuContainer.filterHeight;
        margin: 10;
        radius: popupMenuContainer.delegateRadius;
        anchors.top: parent.top;
        anchors.left: parent.left;
        textSize: popupMenuContainer.textSize;
        fontColor: popupMenuContainer.fontColor;
        onTextEdited: {
            popupMenuContainer.rootItem.currentIndex = -1;
            popupMenuContainer.offset = 0;
            modelFilter.SetData("TextFilter", popupMenuContainer.filterText);
            itemsModel.updateModel(0);
        }
        onAccepted: {
            popupMenuContainer.root.closeDialog();
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
            id: loadedRec;

            anchors.fill: parent;
            opacity: 0.5;
            visible: false;

            Text {

                anchors.centerIn: parent;

                color: Style.textColor;
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                text: "Loaded..."
            }
        }

        ListView {
            id: popupMenuListView;

            width: popupMenuContainer.width;
            height: (popupMenuContainer.countVisibleItem == -1 || popupMenuContainer.countVisibleItem > popupMenuListView.count) ?
                        popupMenuListView.count * popupMenuContainer.itemHeight :
                        popupMenuContainer.countVisibleItem * popupMenuContainer.itemHeight;

            boundsBehavior: Flickable.StopAtBounds;
            clip: true;
            onContentYChanged: {
                if(contentHeight - contentY - popupMenuListView.height == 0){
                    if(!popupMenuContainer.endListStatus && itemsModel.state == "Ready"){
                        console.log("Create additional query")
                        loadedRec.visible = true;
                        popupMenuContainer.offset = popupMenuContainer.offset + popupMenuContainer.count;
                        console.log("Offset = ", popupMenuContainer.offset);
                        itemsModel.updateModel(popupMenuContainer.offset);
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
            viewParams.InsertField("Count", popupMenuContainer.count);
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
                        if (popupMenuContainer.offset == 0){
                            popupMenuContainer.model = dataModelLocal;
                            loadedRec.visible = false;
                        }
                        else{
                            console.log("count items = ", dataModelLocal.GetItemsCount())
                            if(dataModelLocal.GetItemsCount() > 0){
                                for (var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                                    popupMenuContainer.model.InsertNewItem();
                                    dataModelLocal.CopyItemDataToModel(i, popupMenuContainer.model, popupMenuContainer.offset + i);
                                }
                            }
                            else{
                                endListStatus = true;
                            }
                            loadedRec.visible = false;
                        }
                    }
                    dataModelLocal.Refresh();
                }
            }
        }
    }
}
