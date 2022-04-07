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

    property TreeItemModel modelSettings;

    property string currentSettingsBodyId;
    property string currentModeId;

    property int countMainItems: -1;

    Component.onCompleted: {
        settingsQuery.getSettings();
    }

    onFocusChanged: {
        if (preferenceContainer.focus){
            mainPanelColumn.forceActiveFocus();
        }
    }

    onModelSettingsChanged: {
        console.log("PreferenceDialog onModelSettingsChanged");

        var dataModelLocal = preferenceContainer.modelSettings;

        if (!dataModelLocal){
            return;
        }

        if (dataModelLocal.ContainsKey("items")){

            dataModelLocal = dataModelLocal.GetData("items");

            preferenceContainer.countMainItems = dataModelLocal.GetItemsCount();
            mainPanelRepeater.model = dataModelLocal;

            preferenceContainer.settingsBodyChanged();
        }
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
        console.log("PreferenceDialog dataChanged");

        var dataModelLocal = preferenceContainer.modelSettings.GetData("items");

        if (!dataModelLocal){
            return;
        }

        var curBodyId = dataModelLocal.GetData("Id", mainPanelRepeater.selectedIndex);

        if (!curBodyId){
            return;
        }

        if (curBodyId === "General"){
            preferenceContainer.generalSettingsChanged(index, modelElements, activeValue);
        }
        else if (curBodyId === "DBSettings"){
            preferenceContainer.databaseDataChanged(index, modelElements, activeValue);
        }
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
                       dependentPanelLoader.item.currentItemIndex = model.index;
                       dependentPanelLoader.item.itemId = itemId;
                       dependentPanelLoader.item.rootItem = preferenceContainer;
                       dependentPanelLoader.item.delegateItem = dependentPanelRepeater;
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

           if (preferenceContainer.currentModeId !== Style.theme){
               Style.theme = preferenceContainer.currentModeId;
               Style.changeSchemeDesign(preferenceContainer.currentModeId);
           }

           preferenceSaveQuery.save();
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
       }
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
