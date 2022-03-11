import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: preferenceContainer;

    width: 450;
    height: 500;

    radius: 3;

    color: Style.backgroundColor;

    property real backgroundOpacity: 0.4;
    property bool centered: true;

    property Item loaderDialog;

    property TreeItemModel modelSettings;

    property string currentSettingsBodyId;
    property string currentModeId;

    Component.onCompleted: {
        settingsQuery.getSettings();
    }

    onModelSettingsChanged: {
        console.log("PreferenceDialog onModelSettingsChanged");

        var dataModelLocal = preferenceContainer.modelSettings;

        if (!dataModelLocal){
            return;
        }

        if (dataModelLocal.ContainsKey("items")){

            dataModelLocal = dataModelLocal.GetData("items");
            console.log("items contains", dataModelLocal.GetItemsCount());
            mainPanelRepeater.model = dataModelLocal;

            preferenceContainer.settingsBodyChanged();
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

        dataModelLocal = preferenceContainer.modelSettings.GetData("items");

        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++){

            console.log("Name: ", dataModelLocal.GetData("Name", i));

            var settBody = dataModelLocal.GetData("Elements", i);

            for (var j = 0; j < settBody.GetItemsCount(); j++){
                var name = settBody.GetData("Name", j);
                console.log("\tName: ", name);
                var elements = settBody.GetData("Parameters", j);

                var value = settBody.GetData("Value", j);

                if (value !== undefined){
                    console.log("\tValue: ", value);
                }

                for (var k = 0; k < elements.GetItemsCount(); k++){
                    console.log("\t\tName: ", elements.GetData("Name", k));

                    var curValue = elements.GetData("Value", k);

                    if (curValue){
                        console.log("\t\tValue ", curValue);
                    }
                }
            }
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
            modelSettingsBody.SetData("Value", activeValue, index);

            var selectedId = modelElements.GetData("Id", activeValue);
            preferenceContainer.currentModeId = selectedId;

            if (curId === "Mode"){
//                Style.theme = selectedId;
//                stylesQuery.getStyle(selectedId);
            }
            else if (curId === "Language"){

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

    function getThemeColor(colorType, colorKey, themeType) {
        var colorPalette = themeType.GetData("Style").GetData(colorType).GetData(colorKey);
        return themeType.GetData("ColorPalette").GetData(colorPalette);
    }

    function parseStyleTheme(themeType) {
        console.log("PreferenceDialog parseStyleTheme");

        Style.baseColor = preferenceContainer.getThemeColor("ActiveColors", "Base", themeType);
        Style.alternateBaseColor = preferenceContainer.getThemeColor("ActiveColors", "AlternateBase", themeType);
        Style.backgroundColor = preferenceContainer.getThemeColor("ActiveColors", "Background", themeType);
        Style.textColor = preferenceContainer.getThemeColor("ActiveColors", "Text", themeType);
        Style.textSelected = preferenceContainer.getThemeColor("ActiveColors", "TextSelectedBackground", themeType);
        Style.selectedColor = preferenceContainer.getThemeColor("ActiveColors", "ItemSelected", themeType);
        Style.buttonColor = preferenceContainer.getThemeColor("ActiveColors", "HeaderBorder", themeType);
        Style.buttonBorderColor = preferenceContainer.getThemeColor("ActiveColors", "ButtonBorder", themeType);

        Style.disabledInActiveTextColor = preferenceContainer.getThemeColor("DisabledInActiveColors", "Text", themeType);

        Style.hover = preferenceContainer.getThemeColor("ActiveColors", "Hover", themeType);

        Style.imagingToolsGradient0 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient0");
        Style.imagingToolsGradient1 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient1");
        Style.imagingToolsGradient2 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient2");
        Style.imagingToolsGradient3 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient3");
        Style.imagingToolsGradient4 = themeType.GetData("ColorPalette").GetData("ImagingToolsGradient4");

        Style.iconColorOnSelected = preferenceContainer.getThemeColor("IconColor", "OnSelected", themeType);
        Style.tabSelectedColor = preferenceContainer.getThemeColor("ActiveColors", "TabSelected", themeType);
        Style.errorTextColor = preferenceContainer.getThemeColor("ActiveColors", "ErrorText", themeType);

        Style.shadowColor = preferenceContainer.getThemeColor("ActiveColors", "Shadow", themeType);
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

       Repeater {
           id: dependentPanelRepeater;

           width: dependentPanelColumn.width;

           delegate: Item {

               width: dependentPanelColumn.width;
               height: 100;

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
                   }
               }
           }
       }
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
       borderColor: preferenceSaveButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
       backgroundColor: Style.imagingToolsGradient1;

       onClicked: {
           console.log("PreferenceDialog saveButton onClicked");

           if (preferenceContainer.currentModeId !== Style.theme){
               Style.theme = preferenceContainer.currentModeId;
               stylesQuery.getStyle(preferenceContainer.currentModeId);
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
       borderColor: preferenceCloseButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
       backgroundColor: Style.imagingToolsGradient1;

       onClicked: {
           console.log("PreferenceDialog closeButton onClicked");
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
       id: stylesQuery;

       function getStyle(theme){
           var query = Gql.GqlRequest("query", "GetStyle");
           var inputParams = Gql.GqlObject("input");
           inputParams.InsertField("theme");
           inputParams.InsertFieldArgument("theme", theme);
           query.AddParam(inputParams);

           var queryFields = Gql.GqlObject("style");
           queryFields.InsertField("theme");
           queryFields.InsertField("source");
           query.AddField(queryFields);

           var gqlData = query.GetQuery();
           console.log("Preference GqlModel getStyle query ", gqlData);
           this.SetGqlQuery(gqlData);
       }

       onStateChanged: {
           console.log("State:", this.state, stylesQuery);
           if (this.state === "Ready") {
               var dataModelLocal = this.GetData("data");

               if(dataModelLocal.ContainsKey("GetStyle")) {
                   dataModelLocal = dataModelLocal.GetData("GetStyle");
               }

               if(dataModelLocal.ContainsKey("source")){
                   dataModelLocal = dataModelLocal.GetData("source");
                   preferenceContainer.parseStyleTheme(dataModelLocal);
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

               if(dataModelLocal.ContainsKey("GetStyle")) {
//                   dataModelLocal = dataModelLocal.GetData("GetStyle");
               }

               if(dataModelLocal.ContainsKey("source")){
                   dataModelLocal = dataModelLocal.GetData("source");
//                   preferenceContainer.parseStyleTheme(dataModelLocal);
               }
           }
       }
   }
}
