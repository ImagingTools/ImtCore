import QtQuick 2.12
import Qt5Compat.GraphicalEffects 1.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: popupMenuContainer;


    width: itemWidth;
    height: popupMenuListView.height;

    property var model;
    //property TreeItemModel model: TreeItemModel{};
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
    property string excludeFilterPart: "";
    property bool endListStatus: false;
    property bool hiddenBackground: true;
    property bool canClose: true;
    property bool canUpdateModel: true;
    property bool preventFirstLoading: false;

    property alias modelFilterAlias: modelFilter;

    property int elementsCount: -1;
    property int pauseDuration: 500;


    onElementsCountChanged: {
        //console.log("__________ELEMENTS_COUNT____________",elementsCount);
    }

    property bool ready: root && rootItem && gettedParams && filterIdsModel && properties;

    // ID for display in combo box delegates
    property string nameId: "Name";


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

    property int selectedIndex: -1;

    property bool hoverBlocked: true;

    signal finished(string commandId, int index);
    signal endList();
    signal textEdited();
    signal started();


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
        //console.log("_____________POPUP_COMPL_____________", popupMenuContainer.preventFirstLoading);

        modelFilter.AddTreeModel("FilterIds");
        modelFilter.SetData("FilterIds", popupMenuContainer.filterIdsModel)
        modelFilter.AddTreeModel("Sort");

        if(!popupMenuContainer.preventFirstLoading){
            itemsModel.updateModel(0);
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)

    }


    onPropertiesChanged: {
        //console.log("_____________PROPERTIES_CHANGED_____________")

        for (var item = 0; item < popupMenuContainer.properties.GetItemsCount(); item++){
            modelFilter.SetData(popupMenuContainer.properties.GetData("Id", item),  popupMenuContainer.properties.GetData("Value", item));
        }
        //itemsModel.updateModel(0);
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
        Events.subscribeEvent("DialogBackgroundClicked", popupMenuContainer.onBackgroundClicked)

    }


    onFinished: {
        //        if(popupMenuContainer.canClose){
        //            popupMenuContainer.root.closeDialog();
        //        }
    }

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

        //        onTextEdited:  {
        //            console.log("______TEXT_EDITED______")
        //        }
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

            source: "../../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

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

                text: "Нет данных";
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

    function contentYCorrection(down_){
        if(popupMenuContainer.rootItem){
            if(popupMenuContainer.rootItem.selectedIndex >=0){
                var contentY = popupMenuListView.contentY;
                var itemHeight = popupMenuContainer.itemHeight;
                var visibleCount = popupMenuContainer.countVisibleItem;
                var index = popupMenuContainer.rootItem.selectedIndex;
                if(down_){
                    if((index+1) * itemHeight > contentY + visibleCount * itemHeight){
                        popupMenuListView.contentY = (index+1) * itemHeight - visibleCount * itemHeight
                    }
                }
                else {
                    if(index * itemHeight < contentY){
                        popupMenuListView.contentY = index * itemHeight
                    }
                }
            }

        }
    }

    Shortcut {
        sequence: "Escape";
        enabled: true;
        onActivated: {
            root.closeDialog();
        }
    }

    Shortcut {
        sequence: "Return";
        enabled: !filterField.textInputFocus;
        onActivated: {
            if(popupMenuContainer.rootItem){
                if(popupMenuContainer.rootItem.selectedIndex >=0){
                    var id = popupMenuContainer.model.GetData("Id", popupMenuContainer.rootItem.selectedIndex);
                    popupMenuContainer.finished(id, popupMenuContainer.rootItem.selectedIndex);
                }
            }
        }
    }

    Shortcut {
        sequence: "Up";
        enabled: true;
        onActivated: {
            popupMenuContainer.hoverBlocked = true;
            if(popupMenuContainer.rootItem){
                popupMenuContainer.rootItem.hoverBlocked = true;
                if(popupMenuContainer.rootItem.selectedIndex > 0){
                    popupMenuContainer.rootItem.selectedIndex--;
                    popupMenuContainer.contentYCorrection(false);
                }
            }

        }
    }
    Shortcut {
        sequence: "Down";
        enabled: true;
        onActivated: {
            popupMenuContainer.hoverBlocked = true;
            if(popupMenuContainer.rootItem){
                popupMenuContainer.rootItem.hoverBlocked = true;
                if(popupMenuContainer.rootItem.selectedIndex < popupMenuContainer.model.GetItemsCount() - 1){
                    popupMenuContainer.rootItem.selectedIndex++;
                    popupMenuContainer.contentYCorrection(true);
                }
                else if(popupMenuContainer.rootItem.selectedIndex == popupMenuContainer.model.GetItemsCount() - 1){
                    popupMenuContainer.createAdditionalQuery();
                    popupMenuListView.contentY += popupMenuContainer.itemHeight;
                }

            }
        }
    }


//    PauseAnimation {
//        id: contentYCorrectionPause;
//        duration: 200
//    }

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

                            //console.log("count items = ", dataModelLocal.GetItemsCount(), dataModelLocal.toJSON())

                            if(dataModelLocal.GetItemsCount() > 0){
                                for (var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                                    var index_ = popupMenuContainer.model.InsertNewItem();
                                    //dataModelLocal.CopyItemDataToModel(i, popupMenuContainer.model, popupMenuContainer.offset + i);
                                    dataModelLocal.CopyItemDataToModel(i, popupMenuContainer.model, index_);
                                }
                                //popupMenuListView.model = popupMenuContainer.model;
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
