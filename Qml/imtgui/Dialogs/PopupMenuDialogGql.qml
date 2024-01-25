import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

PopupMenuDialog {
    id: popupMenuContainer;

    decorator: Component {
        Item {
            function setTextFocus(focus){
                filterField.setFocus(focus);
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

                onTextChanged:  {

                    if(popupMenuContainer.ready){
                        popupMenuContainer.rootItem.currentIndex = -1;
                        popupMenuContainer.offset = 0;
                        //modelFilter.SetData("TextFilter", popupMenuContainer.filterText);
                        var str = text.replace(popupMenuContainer.excludeFilterPart, "");
                        modelFilter.SetData("TextFilter", str);
                        popupMenuContainer.rootItem.editSignal();
                        pause.stop();
                        pause.start();
                    }
                }
                onAccepted: {
                    popupMenuContainer.rootItem.closeSignal(popupMenuContainer.model);
                    popupMenuContainer.root.closeDialog();
                }

                onTextInputFocusChanged: {
                    popupMenuContainer.rootItem.inFocus = filterField.textInputFocus;
                }
            }

            PauseAnimation {
                id: pause;

                duration: 500;
                onFinished:  {
                    if(popupMenuContainer.canUpdateModel){
                        popupMenuContainer.updateModel(0);
                    }
                }
            }

            Item{
                id: clearContainer;

                anchors.right: filterField.right;
                anchors.verticalCenter: filterField.verticalCenter;
                anchors.rightMargin: 5;

                z: filterField.z +1;

                width: 12;
                height: 12;
                visible: filterField.text !=="";

                Image {
                    id: clearIcon;

                    anchors.fill: parent;

                    source: "../../../../" + "Icons/" + Style.theme + "/" + "Close" + "_On_Normal.svg";

                    sourceSize.width: width;
                    sourceSize.height: height;

                    visible: parent.visible;
                }

                MouseArea {
                    id: clearMouseArea;

                    anchors.fill: parent;
                    hoverEnabled: true;

                    cursorShape: Qt.PointingHandCursor;

                    visible: parent.visible;

                    onClicked: {
                        filterField.text = "";
                    }
                }
            }

            Rectangle {
                id: itemBody;

                anchors.top: filterField.bottom
                anchors.left: parent.left

                width: popupMenuContainer.width;
                height: popupMenuListView.height + noDataRec.height * noDataRec.visible;
                radius: popupMenuContainer.delegateRadius;

                color: Style.baseColor;

                border.width: 1;
                border.color: Style.alternateBaseColor;

                Rectangle{
                    id: noDataRec;

                    width: parent.width;
                    height: 50;
                    radius: parent.radius;
                    color: parent.color;

                    visible: !popupMenuListView.count;

                    Text{
                        id: noDataText;

                        anchors.centerIn: parent;

                        font.pixelSize:  popupMenuContainer.textSize;
                        color: popupMenuContainer.fontColor;

                        text: qsTr("No data")
                    }
                }

                Rectangle{
                    id: loadedRec;

                    anchors.fill: parent;
                    opacity: 0.5;
                    color: "transparent";
                    visible: false;

                    Text {
                        anchors.centerIn: parent;

                        color: Style.textColor;
                        font.pixelSize: Style.fontSize_common;
                        font.family: Style.fontFamily;

                        text: qsTr("Loaded...")
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
                    model: popupMenuContainer.model;

                    onContentYChanged: {
                        if(popupMenuContainer.ready){
                            if(contentHeight - contentY - popupMenuListView.height == 0){
                                popupMenuContainer.createAdditionalQuery();
                            }
                        }
                    }
                    //delegate: PopupMenuDelegate{textSize: popupMenuContainer.textSize}// Delegate Item
                    delegate: popupMenuContainer.delegate;
                }//ListView

                MouseArea{
                    anchors.fill: parent;
                    hoverEnabled: true;
                    visible: popupMenuContainer.rootItem ? popupMenuContainer.rootItem.hoverBlocked :
                                                           popupMenuContainer.hoverBlocked;
                    onPositionChanged: {
                        popupMenuContainer.hoverBlocked = false;
                        if(popupMenuContainer.rootItem){
                            popupMenuContainer.rootItem.hoverBlocked = false;
                        }
                    }
                }
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
        }
    }

    property int filterHeight: 30;
    property int offset: 0;
    property int delegateRadius: 0;
    property int count: 15;
    property string commandId: "";
    property string filterName: "Name";
    property alias filterText: filterField.text;
    property string excludeFilterPart: "";
    property bool endListStatus: false;
    property bool canClose: true;
    property bool canUpdateModel: true;
    property bool preventFirstLoading: false;

    property alias modelFilterAlias: modelFilter;

    property int elementsCount: -1;
    property int pauseDuration: 500;

    property bool ready: popupMenuContainer.root && rootItem && gettedParams && filterIdsModel && properties;

    property Component delegate: PopupMenuDelegate{
        textSize: popupMenuContainer.textSize;
        fontColor: popupMenuContainer.fontColor;

        rootItem: popupMenuContainer.rootItem;
    }

    property TreeItemModel properties: TreeItemModel{};
    property TreeItemModel gettedParams: TreeItemModel{};
    property TreeItemModel filterIdsModel: TreeItemModel{};

    /**
        Count of the visible item, if value = -1 then count unlimited
    */
    property int countVisibleItem: -1;

    signal endList();
    signal textEdited();

    function onBackgroundClicked(){
        popupMenuContainer.root.closeDialog();
    }

    function updateModel(){
        itemsModel.updateModel(0);
    }

    TreeItemModel{
        id: modelFilter;
    }

    Component.onCompleted: {
        modelFilter.AddTreeModel("FilterIds");
        modelFilter.SetData("FilterIds", popupMenuContainer.filterIdsModel)
        modelFilter.AddTreeModel("Sort");

        if(!popupMenuContainer.preventFirstLoading){
            itemsModel.updateModel(0);
        }
    }

    onPropertiesChanged: {
        for (var item = 0; item < popupMenuContainer.properties.GetItemsCount(); item++){
            modelFilter.SetData(popupMenuContainer.properties.GetData("Id", item),  popupMenuContainer.properties.GetData("Value", item));
        }
    }

    function createAdditionalQuery(){
        var elemCountOk = popupMenuContainer.elementsCount >= 0 ? popupMenuContainer.elementsCount > popupMenuContainer.offset : true;
        var ok = !popupMenuContainer.endListStatus
                && itemsModel.state == "Ready"
                && elemCountOk;
        if(ok){
            loadedRec.visible = true;
            popupMenuContainer.offset = popupMenuContainer.offset + popupMenuContainer.count;
            itemsModel.updateModel(popupMenuContainer.offset);
        }
    }

    GqlModel {
        id: itemsModel;

        function updateModel(offsetVar) {
            //console.log("______UPDATE_MODEL_____", offsetVar);
            var query = Gql.GqlRequest("query", popupMenuContainer.commandId);

            var inputParams = Gql.GqlObject("input");
            query.AddParam(inputParams);
            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offsetVar);
            viewParams.InsertField("Count", popupMenuContainer.count);
            viewParams.InsertField("FilterModel");
            var jsonString = modelFilter.toJSON();
            //            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            viewParams.InsertField("FilterModel", jsonString);
            inputParams.InsertFieldObject(viewParams);
            var queryFields = Gql.GqlObject("items");

            for (var item = 0; item < popupMenuContainer.gettedParams.GetItemsCount(); item++){
                var nameParam = popupMenuContainer.gettedParams.GetData("Name", item);
                queryFields.InsertField(nameParam);
            }

            query.AddField(queryFields)
            var gqlData = query.GetQuery();

            //console.log("COMBO_QUERY ", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            //console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){

                let dataModelLocal;

                if (itemsModel.ContainsKey("errors")){
                    return;
                }


                if (itemsModel.ContainsKey("data")){

                    dataModelLocal = itemsModel.GetData("data");

                    if (dataModelLocal.ContainsKey(popupMenuContainer.commandId)){
                        dataModelLocal = dataModelLocal.GetData(popupMenuContainer.commandId);

                        if (dataModelLocal.ContainsKey("notification")){
                            var notifModel = dataModelLocal.GetData("notification");
                            if (notifModel.ContainsKey("ElementsCount")){
                                popupMenuContainer.elementsCount = notifModel.GetData("ElementsCount");
                            }
                            if(notifModel.ContainsKey("Close")){
                                if(notifModel.GetData("Close")){
                                    popupMenuContainer.rootItem.closeFunc();
                                }
                            }
                        }

                        dataModelLocal = dataModelLocal.GetData("items");

                        //console.log(popupMenuContainer.commandId, " = ", dataModelLocal);
                        //console.log("comboModel:::", dataModelLocal.toJSON());
                        if (popupMenuContainer.offset == 0){
                            popupMenuContainer.model = dataModelLocal;
                            //popupMenuListView.model = popupMenuContainer.model;

                            loadedRec.visible = false;
                            popupMenuContainer.endListStatus = false;
                        }
                        else{//OFSET !== 0

                            if(dataModelLocal.GetItemsCount() > 0){
                                for (var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                                    var index_ = popupMenuContainer.model.InsertNewItem();
                                    //dataModelLocal.CopyItemDataToModel(i, popupMenuContainer.model, popupMenuContainer.offset + i);
                                    dataModelLocal.CopyItemDataToModel(i, popupMenuContainer.model, index_);
                                }

                                popupMenuContainer.endListStatus = false;
                            }
                            else{
                                popupMenuContainer.endListStatus = true;
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

