import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: preferenceContainer;

    width: 450;
    height: 480;

    radius: 3;

    clip: true;

    color: Style.backgroundColor;

    property real backgroundOpacity: 0.3;
    property bool centered: true;

    property Item loaderDialog;
    property Item thumbnailItem;

    property TreeItemModel localSettings;
    property TreeItemModel modelSettings: modelSet;

    property string currentSettingsBodyId;
    property string currentModeId;

    property int countMainItems: -1;

    TreeItemModel {
        id: modelSet;
    }

    Component.onCompleted: {
        settingsQuery.getSettings();
    }

    onFocusChanged: {
        if (preferenceContainer.focus){
            mainPanelColumn.forceActiveFocus();
        }
    }

    onLocalSettingsChanged: {
        console.log("PreferenceDialog onLocalSettingsChanged", preferenceContainer.localSettings);

        if (!preferenceContainer.modelSettings){
            return;
        }

        var dataModelLocal = preferenceContainer.modelSettings;
        var allModelItems = dataModelLocal.GetData("items");

        if (!allModelItems){
            allModelItems = dataModelLocal.AddTreeModel("items");
        }

        for (var i = 0; i < preferenceContainer.localSettings.GetItemsCount(); i++){
            console.log("2");
            var id = preferenceContainer.localSettings.GetData("Id", i);
            var name = preferenceContainer.localSettings.GetData("Name", i);
            var elements = preferenceContainer.localSettings.GetData("Elements", i);
            var componentType = preferenceContainer.localSettings.GetData("ComponentType", i);

            var index = allModelItems.InsertNewItem();
            console.log("id", id);
            console.log("name", name);
            console.log("componentType", componentType);
            console.log("index", index);
            allModelItems.SetData("Id", id, index);
            allModelItems.SetData("Name", name, index);
            allModelItems.SetData("ComponentType", componentType, index);

            if (elements){
                allModelItems.SetExternTreeModel("Elements", elements, index)
            }
        }

        dataModelLocal.SetData("items", allModelItems);

        console.log("preferenceContainer.modelSettings", dataModelLocal.toJSON())
        preferenceContainer.modelSettings = dataModelLocal;
        preferenceContainer.modelSettingsChange();
    }

    function modelSettingsChange(){
        console.log("PreferenceDialog modelSettingsChanged");

        if (!preferenceContainer.modelSettings){
            return;
        }
        if (preferenceContainer.modelSettings.ContainsKey("items")){
            var items = preferenceContainer.modelSettings.GetData("items");
            console.log("items", items.toJSON())
            preferenceContainer.countMainItems = items.GetItemsCount();
            mainPanelRepeater.model = items;
            preferenceContainer.settingsBodyChanged();
        }
    }

    onModelSettingsChanged: {
        console.log("PreferenceDialog onModelSettingsChanged");

        //var dataModelLocal = preferenceContainer.modelSettings;

        if (!preferenceContainer.modelSettings){
            return;
        }
        console.log("0");


            var items = preferenceContainer.modelSettings.GetData("items");
            if (preferenceContainer.localSettings){
                console.log("1");
                for (var i = 0; i < preferenceContainer.localSettings.GetItemsCount(); i++){
                    console.log("2");
                    var id = preferenceContainer.localSettings.GetData("Id", i);
                    var name = preferenceContainer.localSettings.GetData("Name", i);
                    var elements = preferenceContainer.localSettings.GetData("Elements", i);
                    var componentType = preferenceContainer.localSettings.GetData("ComponentType", i);

                    var index = items.InsertNewItem();

                    items.SetData("Id", id, index);
                    items.SetData("Name", name, index);
                    items.SetData("ComponentType", componentType, index);

                    if (elements){
                        items.SetExternTreeModel("Elements", elements, index)
                    }
                }
            }
            preferenceContainer.modelSettings.SetData("items", items);
            preferenceContainer.modelSettingsChange();

//            console.log("items", items.toJSON())
//            preferenceContainer.countMainItems = items.GetItemsCount();
//            mainPanelRepeater.model = items;

//            preferenceContainer.settingsBodyChanged();
    }

    Keys.onPressed: {
        console.log("Preference keys pressed")
        if (event.key === Qt.Key_Escape){
            console.log('Key tab was pressed');
            preferenceCloseButton.clicked();
        }
        else if (event.key === Qt.Key_Return){
            preferenceSaveButton.clicked();
            preferenceCloseButton.clicked();
        }
    }

    function dataChanged(index, modelElements, activeValue){
        console.log("PreferenceDialog dataChanged", index, modelElements, activeValue);

        var dataModelLocal = preferenceContainer.modelSettings.GetData("items");

        if (!dataModelLocal){
            return;
        }

        var curBodyId = dataModelLocal.GetData("Id", mainPanelRepeater.selectedIndex);

        if (!curBodyId){
            return;
        }
        console.log("curBodyId", curBodyId);
        if (curBodyId === "General"){
            preferenceContainer.generalSettingsChanged(index, modelElements, activeValue);
        }
        else if (curBodyId === "DBSettings"){
            preferenceContainer.databaseDataChanged(index, modelElements, activeValue);
        }
        else{
            preferenceContainer.settingsLocalChanged(index, activeValue);
        }
    }

    function settingsLocalChanged(index, activeValue){
        console.log("PreferenceDialog settingsLocalChanged", index, activeValue);
        var dataModelLocal = preferenceContainer.modelSettings.GetData("items");

        if (!dataModelLocal){
            return;
        }

        var modelSettingsBody = dataModelLocal.GetData("Elements", mainPanelRepeater.selectedIndex);

        modelSettingsBody.SetData("Value", activeValue, index);
        dataModelLocal.SetData("Elements", modelSettingsBody, mainPanelRepeater.selectedIndex);

        preferenceContainer.modelSettings.SetData("items", dataModelLocal);

        console.log("preferenceContainer.modelSettings", preferenceContainer.modelSettings.toJSON());
    }

    function generalSettingsChanged(index, modelElements, activeValue){
        console.log("PreferenceDialog generalSettingsChanged", index, modelElements, activeValue);
        var dataModelLocal = preferenceContainer.modelSettings.GetData("items");

        if (!dataModelLocal){
            return;
        }

        var modelSettingsBody = dataModelLocal.GetData("Elements", mainPanelRepeater.selectedIndex);

        if (!modelSettingsBody){
            return;
        }

        var curId = modelSettingsBody.GetData("Id", index);

        if (!curId){
            return;
        }

        if (activeValue !== undefined){

            var itemId = modelSettingsBody.GetData("Id", index);
            modelSettingsBody.SetData("Value", activeValue, index);
            var selectedId = modelElements.GetData("Id", activeValue);

            if (itemId === "Mode"){
                preferenceContainer.currentModeId = selectedId;
            }
        }

        modelSettingsBody.SetData("Parameters", modelElements, index);

        dataModelLocal.SetData("Elements", modelSettingsBody, mainPanelRepeater.selectedIndex)

        preferenceContainer.modelSettings.SetData("items", dataModelLocal);
    }

    function databaseDataChanged(index, modelElements){
        console.log("PreferenceDialog databaseDataChanged", index, modelElements);
        var dataModelLocal = preferenceContainer.modelSettings.GetData("items");
        var modelSettingsBody = dataModelLocal.GetData("Elements", mainPanelRepeater.selectedIndex);
        modelSettingsBody.SetData("Parameters", modelElements, index);
        dataModelLocal.SetData("Elements", modelSettingsBody, mainPanelRepeater.selectedIndex)
        preferenceContainer.modelSettings.SetData("items", dataModelLocal);
    }

    function settingsBodyChanged(){
        console.log("PreferenceDialog settingsBodyChanged");

        if (preferenceContainer.modelSettings.ContainsKey("items")){
            var dataModelLocal = preferenceContainer.modelSettings.GetData("items");

            if (dataModelLocal.ContainsKey("Elements", mainPanelRepeater.selectedIndex)){
                preferenceContainer.currentSettingsBodyId = dataModelLocal.GetData("Id", mainPanelRepeater.selectedIndex);
                dataModelLocal = dataModelLocal.GetData("Elements", mainPanelRepeater.selectedIndex);
                dependentPanelRepeater.model = dataModelLocal;
            }
        }
    }

    Rectangle {
        id: topPan;

        width: parent.width;
        height: 40;

        color: Style.baseColor;

        Image {
            id: iconPref;

            anchors.left: topPan.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: topPan.verticalCenter;

            width: 20;
            height: 20;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Settings" + "_" + "On" + "_" + "Normal" + ".svg";

            sourceSize.height: height;
            sourceSize.width: width;
        }

        Text {
            id: name;

            anchors.left: iconPref.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: topPan.verticalCenter;

            text: "Preferences";
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: exit;

            anchors.right: topPan.right;
            anchors.rightMargin: 10;
            anchors.verticalCenter: topPan.verticalCenter;

            width: 20;
            height: 20;

            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

            onClicked: {
                preferenceContainer.loaderDialog.closeItem();
            }
        }
    }

    Rectangle {
        id: mainColumnRect;

        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.top: topPan.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        width: 150;

        color: Style.baseColor;

        Column {
            id: mainPanelColumn;

            anchors.top: mainColumnRect.top;
            anchors.topMargin: 10;
            anchors.horizontalCenter: parent.horizontalCenter;

            width: mainColumnRect.width - 20;

            spacing: 10;

            Keys.onPressed: {
                console.log("PreferenceDialog main Column Key clicked!", preferenceContainer.focus, dependentPanelColumn.currentFocusIndex)

                if(event.key === Qt.Key_Tab) {
                    console.log('Key tab was pressed');
                    dependentPanelRepeater.focusOnNextItem();
                }
                else if (event.key === Qt.Key_Up){
                    console.log('Key up was pressed');

                    if (mainPanelRepeater.selectedIndex == 0){
                        mainPanelRepeater.selectedIndex = preferenceContainer.countMainItems - 1;
                    }
                    else{
                        mainPanelRepeater.selectedIndex--;
                    }
                }
                else if (event.key === Qt.Key_Down){
                    console.log('Key down was pressed');

                    if (mainPanelRepeater.selectedIndex == preferenceContainer.countMainItems - 1){
                        mainPanelRepeater.selectedIndex = 0;
                    }
                    else{
                        mainPanelRepeater.selectedIndex++;
                    }
                }
                else if (event.key === Qt.Key_Escape){
                    console.log('Key esc was pressed');
                    preferenceCloseButton.clicked();
                }
             }

            Repeater {
                id: mainPanelRepeater;

                property int selectedIndex: 0;

                delegate: AuxButton {
                    width: mainPanelColumn.width;
                    height: 35;

                    radius: 3;

                    hasText: true;
                    hasIcon: false;

                    textButton: model.Name;

                    borderColor: mainPanelRepeater.selectedIndex === model.index ? Style.iconColorOnSelected : Style.buttonColor;

                    onClicked: {
                        console.log("PreferenceDialog AuxButton onClicked");
                        mainPanelRepeater.selectedIndex = model.index;

                        if (!mainPanelColumn.focus){
                            mainPanelColumn.forceActiveFocus();
                        }
                    }
                }

                onSelectedIndexChanged: {
                    console.log("PreferenceDialog Repeater onSelectedIndexChanged");
                    preferenceContainer.settingsBodyChanged();
                }
            }
        }
    }

   Column {
       id: dependentPanelColumn;

       anchors.left: mainColumnRect.right;
       anchors.leftMargin: 20;
       anchors.right: preferenceContainer.right;
       anchors.rightMargin: 20;

       anchors.top: topPan.bottom;
       anchors.topMargin: 10;

       height: parent.height;

       property int currentFocusIndex: -1;

       property Item activeItem;

       Repeater {
           id: dependentPanelRepeater;

           width: dependentPanelColumn.width;

           function focusOnNextItem(){
               console.log("Repeater focusOnNextItem");
               if (dependentPanelColumn.activeItem){
                   dependentPanelColumn.activeItem.setFocus(false);
               }

               dependentPanelColumn.currentFocusIndex++;

               if (dependentPanelColumn.currentFocusIndex == dependentPanelRepeater.count){
                   preferenceContainer.focusOnButtons();
               }
               else{
                   dependentPanelColumn.activeItem = dependentPanelRepeater.itemAt(dependentPanelColumn.currentFocusIndex);
                   dependentPanelColumn.activeItem.setFocus(true);
               }
           }

           delegate: Item {
               id: dependentDelegate;

               width: dependentPanelColumn.width;
               height: 100;

               function setFocus(state){
                   console.log("PreferenceDialog setFocus");
                   if (state === true){
                      dependentPanelLoader.item.forceActiveFocus();
                   }
                   dependentPanelLoader.item.active = state;
               }

               Text {
                   id: titleItem;

                   text: model.Name;

                   font.pixelSize: Style.fontSize_common;
                   color: Style.textColor;
                   font.family: Style.fontFamily;
               }

               Rectangle {
                   id: rectSeparator;

                   anchors.top: titleItem.bottom;
                   anchors.topMargin: 5;

                   width: parent.width;
                   height: 1;

                   color: Style.buttonBorderColor;
               }

               Loader {
                   id: dependentPanelLoader;

                   anchors.top: rectSeparator.bottom;
                   anchors.topMargin: 15;

                   Component.onCompleted: {
                       var componentType = model.ComponentType;
                       var itemId = model.Id;

                       if (componentType === "ComboBox"){
                           dependentPanelLoader.source = "SettingsComboBox.qml";
                           dependentPanelLoader.item.currentValue = model.Value;
                           dependentPanelLoader.item.modelElements = model.Parameters;
                           dependentPanelLoader.item.width = dependentPanelColumn.width / 2;
                       }
                       else if (componentType === "DatabaseSettingsInput"){
                           dependentPanelLoader.source = "DatabaseInput.qml";
                           dependentPanelLoader.item.width = dependentPanelColumn.width;
                           dependentPanelLoader.item.modelDatabase = model.Parameters;
                       }
                       else if (componentType === "TextInput"){
                           dependentPanelLoader.source = "SettingsTextInput.qml";
                           dependentPanelLoader.item.width = dependentPanelColumn.width;
                           dependentPanelLoader.item.itemId = itemId;
                           dependentPanelLoader.item.value = model.Value;
                       }

                       dependentPanelLoader.item.itemId = itemId;
                       dependentPanelLoader.item.rootItem = preferenceContainer;
                       dependentPanelLoader.item.delegateItem = dependentPanelRepeater;
                       dependentPanelLoader.item.currentItemIndex = model.index;
                   }
               }
           }
       }
   }

   function focusOnButtons(){
       preferenceSaveButton.forceActiveFocus();
   }

   AuxButton {
       id: preferenceSaveButton;

       anchors.right: preferenceCloseButton.left;
       anchors.rightMargin: 10;
       anchors.bottom: parent.bottom;
       anchors.bottomMargin: 10;

       width: 70;
       height: 25;

       hasText: true;
       hasIcon: false;

       textButton: "Apply";
       borderColor: (preferenceSaveButton.highlighted || preferenceSaveButton.focus) ? Style.iconColorOnSelected : Style.buttonColor;
       backgroundColor: Style.imagingToolsGradient1;

       Keys.onPressed: {
           console.log("PreferenceDialog preferenceSaveButton keys pressed")
           if (event.key === Qt.Key_Tab){
               console.log('Key tab was pressed');
               preferenceCloseButton.forceActiveFocus();
           }
           else if (event.key === Qt.Key_Return){
               console.log('Key return was pressed');
               preferenceSaveButton.clicked();
           }
       }

       onClicked: {
           console.log("PreferenceDialog saveButton onClicked", preferenceContainer.currentModeId, Style.theme);

           if (thumbnailItem.serverIsConnection){
               if (preferenceContainer.currentModeId !== Style.theme){
                   Style.theme = preferenceContainer.currentModeId;
                   Style.changeSchemeDesign(preferenceContainer.currentModeId);
               }

               preferenceSaveQuery.save();
           }
           else{

           }
       }
   }

   AuxButton {
       id: preferenceCloseButton;

       anchors.right: parent.right;
       anchors.rightMargin: 20;
       anchors.bottom: parent.bottom;
       anchors.bottomMargin: 10;

       width: 70;
       height: 25;

       hasText: true;
       hasIcon: false;

       textButton: "Close";
       borderColor: (preferenceCloseButton.highlighted || preferenceCloseButton.focus) ? Style.iconColorOnSelected : Style.buttonColor;
       backgroundColor: Style.imagingToolsGradient1;

       Keys.onPressed: {
           console.log("PreferenceDialog preferenceCloseButton keys pressed")
           if (event.key === Qt.Key_Tab){
               console.log('Key tab was pressed');
               preferenceContainer.forceActiveFocus();
               dependentPanelColumn.currentFocusIndex = -1;
           }
           else if (event.key === Qt.Key_Return){
               console.log('Key return was pressed');
               preferenceCloseButton.clicked();
           }
       }

       onClicked: {
           console.log("PreferenceDialog closeButton onClicked");
           preferenceContainer.thumbnailItem.setFocusOnMenuPanel();
           preferenceContainer.loaderDialog.closeItem();
       }
   }

   GqlModel {
       id: settingsQuery;

       function getSettings() {
           var query = Gql.GqlRequest("query", "GetSettings");

           var queryFields = Gql.GqlObject("items");

           queryFields.InsertField("Mode");
           queryFields.InsertField("Language");
           queryFields.InsertField("DBSettings");

           query.AddField(queryFields);

           var gqlData = query.GetQuery();
           console.log("PreferenceDialog GqlModel getSettings query ", gqlData);
           this.SetGqlQuery(gqlData);
       }

       onStateChanged: {
           console.log("State:", this.state, settingsQuery);
           var dataModelLocal;

           if (settingsQuery.ContainsKey("errors")){
               dataModelLocal = settingsQuery.GetData("errors");

               return;
           }

           if (settingsQuery.ContainsKey("data")){
                dataModelLocal = settingsQuery.GetData("data");

               if (dataModelLocal.ContainsKey("GetSettings")){
                   dataModelLocal = dataModelLocal.GetData("GetSettings");

                   preferenceContainer.modelSettings = dataModelLocal;
               }
           }

//           if (!dataModelLocal){
//               emptyModel.AddTreeModel("items");
//               emptyModel.SetIsArray(true);
//               preferenceContainer.modelSettings = emptyModel;
//           }
       }
   }

   TreeItemModel {
       id: emptyModel;
   }

   GqlModel {
       id: preferenceSaveQuery;

       function save(){
           var query = Gql.GqlRequest("mutation", "SaveSettings");

           var inputParams = Gql.GqlObject("input");

           var jsonString = preferenceContainer.modelSettings.toJSON();
           jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

           inputParams.InsertField("Item");
           inputParams.InsertFieldArgument ("Item", jsonString);
           query.AddParam(inputParams);

           var queryFields = Gql.GqlObject("items");

           queryFields.InsertField("Mode");
           queryFields.InsertField("Language");
           queryFields.InsertField("DBSettings");
           query.AddField(queryFields);

           var gqlData = query.GetQuery();
           console.log("Preference GqlModel SaveSettings query ", gqlData);
           this.SetGqlQuery(gqlData);
       }

       onStateChanged: {
           console.log("State:", this.state, preferenceSaveQuery);
           if (this.state === "Ready") {
               var dataModelLocal = this.GetData("data");
           }
       }
   }
}
