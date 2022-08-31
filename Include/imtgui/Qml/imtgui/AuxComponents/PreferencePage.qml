import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3

Rectangle {
    id: container;

    color: "#80808080"

    property TreeItemModel serverModel;
    property TreeItemModel localModel;

    TreeItemModel {
        id: commonModel;
    }

    onServerModelChanged: {
        console.log("PreferenceDialog onServerModelChanged");

        for (let i = 0; i < serverModel.GetItemsCount(); i++){
            let pageId = serverModel.GetData("Id", i);
            let index = getPageIndexByPageId(pageId)

            if (index < 0){
                index = commonModel.InsertNewItem();
            }

            commonModel.CopyItemDataFromModel(index, serverModel, i);
        }

        updateGui();
    }

    onLocalModelChanged: {
        console.log("PreferenceDialog onLocalModelChanged");

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageId = localModel.GetData("Id", i);
            let index = getPageIndexByPageId(pageId)

            if (index < 0){
                index = commonModel.InsertNewItem();
            }
            commonModel.CopyItemDataFromModel(index, localModel, i);
        }

        updateGui();
    }

    onVisibleChanged: {
        if (!visible){
            settingsQuery.getSettings();
        }
        else{
            mainPanel.selectedIndex = 0;
            bodyPanelRepeater.model = commonModel.GetData("Elements");
        }
    }

    function updateModel(){
        settingsQuery.getSettings();
    }

    function serverModelUpdated(){
        console.log("serverModelUpdated");
        console.log(serverModel.toJSON());
    }

    function localModelUpdated(){
        console.log("localModelUpdated");
        console.log(localModel.toJSON());
    }

    function updateGui(){
        mainPanelRepeater.model = commonModel;
    }

    function getPageIndexByPageId(pageId){
        for (let i = 0; i < commonModel.GetItemsCount(); i++){
            let id = commonModel.GetData("Id", i);
            if (id == pageId){
                return i;
            }
        }

        return -1;
    }

    function getInstanceMask(){
        console.log("Preferences getInstanceMask");
        let generalPageIndex = getPageIndexByPageId("General");
        if (generalPageIndex >= 0){
            var elements = commonModel.GetData("Elements", generalPageIndex);

            for (let i = 0; i < elements.GetItemsCount(); i++){
                let elementId = elements.GetData("Id", i);
                if (elementId === "InstanceMask"){
                    let elementValue = elements.GetData("Value", i);

                    return elementValue;
                }
            }
        }

        return null;
    }

    function getDesignScheme(){
        console.log("Preferences getDesignScheme");
        let generalPageIndex = getPageIndexByPageId("General");
        if (generalPageIndex >= 0){
            var elements = commonModel.GetData("Elements", generalPageIndex);

            for (let i = 0; i < elements.GetItemsCount(); i++){
                let elementId = elements.GetData("Id", i);
                if (elementId === "Mode"){
                    let elementValue = elements.GetData("Value", i);
                    let parameters = elements.GetData("Parameters", i);

                    let scheme = parameters.GetData("Id", elementValue);

                    return scheme;
                }
            }
        }

        return null;
    }

    function getSelectedLanguage(){
        console.log("Preferences getSelectedLanguage");
        let generalPageIndex = getPageIndexByPageId("General");
        if (generalPageIndex >= 0){
            var elements = commonModel.GetData("Elements", generalPageIndex);

            for (let i = 0; i < elements.GetItemsCount(); i++){
                let elementId = elements.GetData("Id", i);
                if (elementId === "Language"){
                    let elementValue = elements.GetData("Value", i);
                    let parameters = elements.GetData("Parameters", i);

                    let scheme = parameters.GetData("Id", elementValue);

                    return scheme;
                }
            }
        }

        return null;
    }

    function openFileDialog(){
        fileDialogSave.open();
    }

    RemoteFileController {
        id: remoteFileController;
    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");
        selectExisting: false;
        folder: shortcuts.home;

        nameFilters: ["Text files (*.txt)", "All files (*)"];

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.fileUrl.toString().replace(pathDir + "/", '');
            var id = "";

            if (fileName == ""){
                fileName = {};
                fileName["name"] = "LisaServerLog.txt";
            }

            remoteFileController.GetFile(id, fileName);
        }
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        width: 450;
        height: 480;

        radius: 3;

        color: Style.backgroundColor;

        TopPanelDialog {
            id: topPanelDialog;

            width: parent.width;
            height: 40;

            title: qsTr("Preferences");

            onCloseButtonClicked: {
                container.visible = false;
            }
        }

        Rectangle {
            id: mainPanelBackground;

            anchors.top: topPanelDialog.bottom;
            anchors.left: parent.left;
            anchors.bottom: buttonsDialog.top;
            anchors.margins: 10;

            width: 150;
            height: parent.height;

            color: Style.baseColor;

            Column {
                id: mainPanel;

                anchors.fill: parent;
                anchors.topMargin: 10;

                property int selectedIndex: -1;

                spacing: 5;

                Repeater {
                    id: mainPanelRepeater;

                    delegate: AuxButton {

                        anchors.left: parent.left;
                        anchors.leftMargin: 10;
                        anchors.right: parent.right;
                        anchors.rightMargin: 10;

                        height: 35;

                        radius: 3;

                        hasText: true;
                        hasIcon: false;

                        textButton: model.Name;

                        borderColor: mainPanel.selectedIndex == model.index ? Style.iconColorOnSelected : Style.buttonColor;

                        Component.onCompleted: {
                            if (model.index === 0){
                                clicked();
                            }
                        }

                        onClicked: {
                            if (mainPanel.selectedIndex !== model.index){
                                bodyPanelRepeater.model = model.Elements;
                                mainPanel.selectedIndex = model.index;
                            }
                        }
                    }
                }

                onSelectedIndexChanged: {}
            }
        }

        Column {
            id: bodyPanel;

            anchors.top: topPanelDialog.bottom;
            anchors.topMargin: 10;
            anchors.left: mainPanelBackground.right;
            anchors.leftMargin: 10;
            anchors.right: parent.right;
            anchors.rightMargin: 10;
            anchors.bottom: parent.bottom;

            height: parent.height;

            Repeater {
                id: bodyPanelRepeater;

                delegate: Item {

                    width: bodyPanel.width;
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
                        id: loader;

                        anchors.top: rectSeparator.bottom;
                        anchors.topMargin: 15;

                        Component.onCompleted: {
                            var componentType = model.ComponentType;

                            if (componentType === "ComboBox"){
                                loader.source = "SettingsComboBox.qml";
                                loader.item.width = bodyPanel.width / 2;
                            }
                            else if (componentType === "DatabaseSettingsInput"){
                                loader.source = "DatabaseInput.qml";
//                                loader.item.width = bodyPanel.width / 2;
                            }
                            else if (componentType === "TextInput"){
                                loader.source = "SettingsTextInput.qml";
                                loader.item.width = bodyPanel.width;
                            }
                            else if (componentType === "Button"){
                                loader.source = "SettingsButton.qml";
                                loader.item.width = bodyPanel.width / 3;
                            }
                            else if (componentType === "TextLabel"){
                                loader.source = "SettingsTextLabel.qml";

                                loader.item.width = bodyPanel.width / 3;
                            }

                            if (loader.item.parameters){
                                loader.item.parameters = model.Parameters;
                            }

                            loader.item.width = bodyPanel.width / 2;
                        }
                    }
                }
            }
        }

        ButtonsDialog {
            id: buttonsDialog;

            anchors.right: parent.right;
            anchors.rightMargin: 10;
            anchors.bottom: parent.bottom;
            anchors.bottomMargin: 10;

            Component.onCompleted: {

                addButton({"Id":"Apply", "Name":"Apply", "Enabled": true});
                addButton({"Id":"Cancel", "Name":"Cancel", "Enabled": true});

            }

            onButtonClicked: {
                console.log("ButtonsDialog onButtonClicked", buttonId);

                if (buttonId == "Cancel"){
                    container.visible = false;
                }
                else if (buttonId == "Apply"){

                    preferenceSaveQuery.save();

                    //Сигнал на обновление LocalModel
                    window.settingsUpdate();

//                    window.updateModels();
                }
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
                        dataModelLocal = dataModelLocal.GetData("items");

                        serverModel = dataModelLocal;

                        let scheme = getDesignScheme();
                        Style.changeSchemeDesign(scheme);

                        let language = getSelectedLanguage();
                        context.language = language;
                    }
                }
            }
        }//GetSettings

        GqlModel {
            id: preferenceSaveQuery;

            function save(){
                var query = Gql.GqlRequest("mutation", "SaveSettings");

                var inputParams = Gql.GqlObject("input");
                var jsonString = serverModel.toJSON();
                jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

                inputParams.InsertField ("Item", jsonString);
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


                    if (this.ContainsKey("errors")){

                        return;
                    }

                    if (this.ContainsKey("data")){

                        let dataModelLocal = this.GetData("data");

                        let scheme = getDesignScheme();
                        Style.changeSchemeDesign(scheme);
                    }
                }
            }
        }
    }
}
