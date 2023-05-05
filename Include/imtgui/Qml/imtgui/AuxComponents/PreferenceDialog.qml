import QtQuick 2.0
import QtQuick 2.15
import Acf 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3
import QtQuick.Dialogs 6.2

Rectangle {
    id: preferenceContainer;

    width: 450;
    height: 480;

    radius: 3;

    clip: true;
    visible: false;

    color: Style.backgroundColor;

    property real backgroundOpacity: 0.3;
    property bool centered: true;

    property Item loaderDialog;

    property TreeItemModel localSettings;
    property TreeItemModel serverSettings;

    property string currentSettingsBodyId;
    property string currentModeId: "";
    property string selectedLanguage: "";
    property string networkUrl;

    property int countMainItems: -1;

    TreeItemModel {
        id: globalSettings;
    }

    onCurrentModeIdChanged: {
        console.log("PreferenceDialog onCurrentModeIdChanged", preferenceContainer.currentModeId);
    }

    Component.onCompleted: {
        console.log("PreferenceDialog onCompleted");
        //settingsQuery.getSettings();
    }

    onFocusChanged: {
        if (preferenceContainer.focus){
            mainPanelColumn.forceActiveFocus();
        }
    }

    function loadSettings(){
      //  preferenceContainer.serverSettings = 0;
        settingsQuery.getSettings();
    }

    function modelSettingsChange(){
        console.log("PreferenceDialog modelSettingsChanged");

        preferenceContainer.countMainItems = globalSettings.GetItemsCount();
        mainPanelRepeater.model = 0;
        mainPanelRepeater.model = globalSettings;
        preferenceContainer.settingsBodyChanged();
    }

    onLocalSettingsChanged: {
        console.log("PreferenceDialog onLocalSettingsChanged", preferenceContainer.localSettings.toJSON());
        if (preferenceContainer.localSettings){
            for (var i = 0; i < preferenceContainer.localSettings.GetItemsCount(); i++){
                var index = globalSettings.InsertNewItem();
                var id = preferenceContainer.localSettings.GetData("Id", i);
                globalSettings.SetData("Id", id, index);
                globalSettings.SetData("Name", preferenceContainer.localSettings.GetData("Name", i), index);
                globalSettings.SetData("ComponentType", preferenceContainer.localSettings.GetData("ComponentType", i), index);
                var elements = preferenceContainer.localSettings.GetData("Elements", i);

                if (elements){
                    preferenceContainer.networkUrl = elements.GetData("Value");
                    globalSettings.SetData("Elements", elements, index);
                }
            }
        }
        preferenceContainer.modelSettingsChange();
    }

    onServerSettingsChanged: {
        console.log("PreferenceDialog onModelSettingsChanged");

        if (preferenceContainer.serverSettings){
            if (preferenceContainer.serverSettings.ContainsKey("items")){
                var serverItems = preferenceContainer.serverSettings.GetData("items");
                for (let i = 0; i < serverItems.GetItemsCount(); i++){
                    let id = serverItems.GetData("Id", i);
                    let index = preferenceContainer.getPageIndexByPageId(id);

                    if (index < 0){
                        index = globalSettings.InsertNewItem();
                    }

                    globalSettings.SetData("Id", id, index);
                    globalSettings.SetData("Name", serverItems.GetData("Name", i), index);
                    globalSettings.SetData("ComponentType", serverItems.GetData("ComponentType", i), index);
                    globalSettings.SetData("Elements", serverItems.GetData("Elements", i), index);
                }
            }
        }
        preferenceContainer.modelSettingsChange();
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

//    Shortcut {
//        sequence: "Ctrl+S";
//        onActivated: {
//            console.log("Ctrl+S pressed");
//            preferenceSaveButton.clicked();
//        }
//    }

    function dataChanged(index, value){
        console.log("PreferenceDialog dataChanged", index, value);
        var modelSettingsBody = globalSettings.GetData("Elements", mainPanelRepeater.selectedIndex);
        modelSettingsBody.SetData("Value", value, index);

        let elementId = modelSettingsBody.GetData("Id", index);
        if (elementId == "Mode"){
            let parameters = modelSettingsBody.GetData("Parameters", index);
            let parameterId = parameters.GetData("Id", value);

            preferenceContainer.currentModeId = parameterId;
        }
    }

    function settingsBodyChanged(){
        console.log("PreferenceDialog settingsBodyChanged");

        if (globalSettings.ContainsKey("Elements", mainPanelRepeater.selectedIndex)){
            preferenceContainer.currentSettingsBodyId = globalSettings.GetData("Id", mainPanelRepeater.selectedIndex);
            dependentPanelRepeater.model = globalSettings.GetData("Elements", mainPanelRepeater.selectedIndex)
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

            text: qsTr("Preferences");
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
//                preferenceContainer.loaderDialog.closeItem();
                thumbnailDecoratorContainer.setPreferencesVisible(false);
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
//               if (dependentPanelColumn.activeItem){
//                   dependentPanelColumn.activeItem.setFocus(false);
//               }

//               dependentPanelColumn.currentFocusIndex++;

//               if (dependentPanelColumn.currentFocusIndex == dependentPanelRepeater.count){
//                   preferenceContainer.focusOnButtons();
//               }
//               else{
//                   dependentPanelColumn.activeItem = dependentPanelRepeater.itemAt(dependentPanelColumn.currentFocusIndex);
//                   dependentPanelColumn.activeItem.setFocus(true);
//               }
           }

           delegate: Item {
               id: dependentDelegate;

               width: dependentPanelColumn.width;
               height: 100;

               function setFocus(state){
                   console.log("PreferenceDialog setFocus");
//                   if (state === true){
//                      dependentPanelLoader.item.forceActiveFocus();
//                   }
//                   dependentPanelLoader.item.active = state;
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
                       else if (componentType === "Button"){
                           dependentPanelLoader.source = "SettingsButton.qml";
                           dependentPanelLoader.item.width = dependentPanelColumn.width / 3;
                           dependentPanelLoader.item.itemId = itemId;
                           dependentPanelLoader.item.value = model.Value;
                       }
                       else if (componentType === "TextLabel"){
                           dependentPanelLoader.source = "SettingsTextLabel.qml";
                           dependentPanelLoader.item.width = dependentPanelColumn.width / 3;
                           dependentPanelLoader.item.itemId = itemId;
                           dependentPanelLoader.item.value = model.Value;
                       }

                       dependentPanelLoader.item.itemId = itemId;
                       dependentPanelLoader.item.rootItem = preferenceContainer;

                       if (dependentPanelLoader.item.delegateItem){
                           dependentPanelLoader.item.delegateItem = dependentPanelRepeater;
                       }

                       dependentPanelLoader.item.index = model.index;
                   }
               }
           }
       }
   }

   function focusOnButtons(){
       preferenceSaveButton.forceActiveFocus();
   }

   function getPageIndexByPageId(pageId){
       for (let i = 0; i < globalSettings.GetItemsCount(); i++){
           let id = globalSettings.GetData("Id", i);
           if (id == pageId){
               return i;
           }
       }

       return -1;
   }

   function getInstanceMask(){
       console.log("Preferences getInstanceMask");
       let generalPageIndex = preferenceContainer.getPageIndexByPageId("General");

       if (generalPageIndex >= 0){
           var elements = globalSettings.GetData("Elements", generalPageIndex);

           for (let i = 0; i < elements.GetItemsCount(); i++){
               let elementId = elements.GetData("Id", i);
               if (elementId == "InstanceMask"){
                   let elementValue = elements.GetData("Value", i);

                   return elementValue;
               }
           }
       }

       return null;
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

       textButton: qsTr("Apply");
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

           if (thumbnailDecoratorContainer.serverIsConnection){
               if ((preferenceContainer.currentModeId == "Light" ||
                       preferenceContainer.currentModeId == "Dark") &&
                       preferenceContainer.currentModeId !== Style.theme){

                   console.log("preferenceContainer.currentModeId", preferenceContainer.currentModeId);
                   Style.theme = preferenceContainer.currentModeId;
                   Style.changeSchemeDesign(preferenceContainer.currentModeId);
               }

               console.log("preferenceContainer.serverSettings", preferenceContainer.serverSettings.toJSON());

               preferenceSaveQuery.save();
           }

           let localPageIndex = preferenceContainer.getPageIndexByPageId("NetworkSettings");
           if (localPageIndex >= 0){
               var elements = globalSettings.GetData("Elements", localPageIndex);

               let newNetworkUrl = elements.GetData("Value");

               if (newNetworkUrl != preferenceContainer.networkUrl){
                   preferenceContainer.networkUrl = newNetworkUrl
                   thumbnailDecoratorContainer.localSettings.SetData("Elements", elements);
               }
           }

//           if (preferenceContainer.selectedLanguage != "" && preferenceContainer.selectedLanguage != Style.language){
//               Style.language = preferenceContainer.selectedLanguage;
//               context.language = preferenceContainer.selectedLanguage;
//               thumbnailDecoratorContainer.updateModels();
//           }

//           preferenceSaveButton.enabled = false;
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

       textButton: qsTr("Close");
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
           thumbnailDecoratorContainer.setPreferencesVisible(false);
//           preferenceContainer.thumbnailItem.setFocusOnMenuPanel();
//           preferenceContainer.loaderDialog.closeItem();
       }
   }

   GqlModel {
       id: settingsQuery;

       function getSettings() {
           var query = Gql.GqlRequest("query", "GetSettings");

           var inputParams = Gql.GqlObject("input");
           inputParams.InsertField ("LanguageId", Style.language);
           query.AddParam(inputParams);

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
                   preferenceContainer.serverSettings = dataModelLocal;
               }
           }
//           if (!dataModelLocal && preferenceContainer.serverSettings){
//                preferenceContainer.serverSettings.Clear();
//           }

       }
   }

   GqlModel {
       id: preferenceSaveQuery;

       function save(){
           if (!preferenceContainer.serverSettings){
               return;
           }
           var query = Gql.GqlRequest("mutation", "SaveSettings");
//           console.log("preferenceContainer.serverSettings", preferenceContainer.serverSettings.toJSON())

           var inputParams = Gql.GqlObject("input");
           var jsonString = preferenceContainer.serverSettings.toJSON();
           jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

           inputParams.InsertField ("Item", jsonString);
           query.AddParam(inputParams);

           var queryFields = Gql.GqlObject("items");

           queryFields.InsertField("Mode");
           queryFields.InsertField("Language");
           queryFields.InsertField("DBSettings");
           queryFields.InsertField("NetworkSettings");
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

   function openFileDialog(){
       fileDialogSave.open();
//       fileQuery.getFile("LisaServerLog.txt");
   }

   RemoteFileController {
       id: remoteFileController;

       onFileDownloaded: {
           console.log('onFileDownloaded', filePath);
          // console.log('onopening file', remoteFileController.OpenFile(filePath));
       }
   }

   FileDialog {
       id: fileDialogSave;

       title: qsTr("Save file");
       selectExisting: false;
       folder: shortcuts.home;

       nameFilters: ["Text files (*.txt)", "All files (*)"];

       onAccepted: {
           console.log("You chose: " + fileDialogSave.fileUrl);
           console.log("You chose folder: " + fileDialogSave.folder);

           var pathDir = fileDialogSave.folder.toString();
           remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
           var fileName = fileDialogSave.fileUrl.toString().replace(pathDir + "/", '');

           console.log("You chose file name: ", fileName);
           var id = "";

           if (fileName == ""){
               fileName = {};
               fileName["name"] = "LisaServerLog.txt";
           }

           console.log("id ",id);
           console.log("fileName ",fileName);
           remoteFileController.GetFile(id, fileName);
       }
   }

   GqlModel {
       id: fileQuery;

       function getFile(name){
           var query = Gql.GqlRequest("query", "GetFile");
           var inputParams = Gql.GqlObject("input");
           inputParams.InsertField("name", name);
           query.AddParam(inputParams);

           var queryFields = Gql.GqlObject("file");
           queryFields.InsertField("name");
           queryFields.InsertField("data");
           query.AddField(queryFields);

           var gqlData = query.GetQuery();
           console.log("fileQuery gqlData", gqlData);
           this.SetGqlQuery(gqlData);
       }

       onStateChanged: {
           console.log("State:", this.state, fileQuery);
           if (this.state === "Ready") {

               var dataModelLocal;

               if (fileQuery.ContainsKey("errors")){
                   return;
               }

               if (fileQuery.ContainsKey("data")){
                   dataModelLocal = styleQuery.GetData("data");

                   if(dataModelLocal.ContainsKey("FileData")) {
                       let fileData = dataModelLocal.GetData("FileData");
                       fileDialogSave.open();
                   }
               }
           }
       }
   }
}
