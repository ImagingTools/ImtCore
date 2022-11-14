import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3
import "../../Acf/core.js" as Lodash

Rectangle {
    id: container;

    color: Style.dialogBackgroundColor;

    property Item root;

    property TreeItemModel serverModel: TreeItemModel {}
    property TreeItemModel localModel: TreeItemModel {}

    property TreeItemModel commonModel: TreeItemModel {}

    onVisibleChanged: {
        if (visible){
            if (settingsProvider.localModel){
                let localModelJson = settingsProvider.localModel.toJSON();
                localModel.Parse(localModelJson);

                updateCommonModel(localModel);
            }

            if (settingsProvider.serverModel){
                let serverModelJson = settingsProvider.serverModel.toJSON();
                serverModel.Parse(serverModelJson);

                updateCommonModel(serverModel);
            }

            updateGui();
        }
        else{
            clearModels();
        }
    }

    function clearModels(){
        serverModel.Clear();
        localModel.Clear();

        mainPanelRepeater.model = 0;
        bodyPanelRepeater.model = 0;
    }

    function updateGui(){
        mainPanelRepeater.model = commonModel;

        mainPanel.selectedIndex = 0;
        bodyPanelRepeater.model = commonModel.GetData("Elements");
    }

    function updateCommonModel(externModel){
        for (let i = 0; i < externModel.GetItemsCount(); i++){
            let pageId = externModel.GetData("Id", i);
            let index = getPageIndexByPageId(pageId)

            if (index < 0){
                index = commonModel.InsertNewItem();
            }

            commonModel.CopyItemDataFromModel(index, externModel, i);
        }
    }

    function getPageIndexByPageId(pageId, model){
        console.log("Preferences getPageIndexByPageId", pageId);

        if (!model){
            model = commonModel;
        }

        for (let i = 0; i < model.GetItemsCount(); i++){
            let id = model.GetData("Id", i);
            if (id == pageId){
                return i;
            }
        }

        return -1;
    }

    function openFileDialog(){
        fileDialogSave.open();
    }

    function getDirtyPagesFromLocalModel(){
        let pageIds = []

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageCopyModel = localModel.GetModelFromItem(i);
            let pageModel = settingsProvider.localModel.GetModelFromItem(i);

            let pageCopyModelJson = pageCopyModel.toJSON();
            let pageModelJson = pageModel.toJSON();

            console.log("pageCopyModelJson", pageCopyModelJson);
            console.log("pageModelJson", pageModelJson);

            if (_.isEqual(pageCopyModelJson, pageModelJson)){
                console.log("EQUAL");
            }
            else{
                console.log("NOT EQUAL");
                let pageId = localModel.GetData("Id", i)
                pageIds.push(pageId);
            }
        }

        return pageIds;
    }

    function getDirtyPagesFromServerModel(){
        let pageIds = []

        for (let i = 0; i < serverModel.GetItemsCount(); i++){
            let pageCopyModel = serverModel.GetModelFromItem(i);
            let pageModel = settingsProvider.serverModel.GetModelFromItem(i);

            let pageCopyModelJson = pageCopyModel.toJSON();
            let pageModelJson = pageModel.toJSON();

            if (!_.isEqual(pageCopyModelJson, pageModelJson)){
                let pageId = serverModel.GetData("Id", i)
                pageIds.push(pageId);
            }
        }

        return pageIds;
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
                buttonsDialog.buttonClicked("Cancel");
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

        Flickable {
            anchors.top: topPanelDialog.bottom;
            anchors.topMargin: 10;
            anchors.left: mainPanelBackground.right;
            anchors.leftMargin: 10;
            anchors.right: parent.right;
            anchors.rightMargin: 10;
            anchors.bottom: parent.bottom;
            anchors.bottomMargin: 50;

            clip: true;

//            height: parent.height;

            contentWidth: width;
            contentHeight: height + 100;

            boundsBehavior: Flickable.StopAtBounds;

            Column {
                id: bodyPanel;

//                width: 200;

                anchors.fill: parent;

//                anchors.top: topPanelDialog.bottom;
//                anchors.topMargin: 10;
//                anchors.left: mainPanelBackground.right;
//                anchors.leftMargin: 10;
//                anchors.right: parent.right;
//                anchors.rightMargin: 10;
//                anchors.bottom: parent.bottom;

//                height: parent.height;

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
                                    loader.item.width = bodyPanel.width;
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
                            }
                        }
                    }
                }
            }//Column
        }//Flickable

        ButtonsDialog {
            id: buttonsDialog;

            anchors.right: parent.right;
            anchors.rightMargin: 10;
            anchors.bottom: parent.bottom;
            anchors.bottomMargin: 10;

            Component.onCompleted: {
                addButton({"Id":"Apply", "Name": qsTr("Apply"), "Enabled": true});
                addButton({"Id":"Cancel", "Name": qsTr("Cancel"), "Enabled": true});
            }

            onButtonClicked: {
                console.log("ButtonsDialog onButtonClicked", buttonId);

                if (buttonId == "Cancel"){
                    container.visible = false;
                }
                else if (buttonId == "Apply"){
                    let serverPageIds = getDirtyPagesFromServerModel();

                    if (serverPageIds.length > 0){
                        let serverModelJson = serverModel.toJSON();
                        settingsProvider.serverModel.Parse(serverModelJson);

                        settingsProvider.saveServerModel(serverPageIds);
                    }

                    let localPageIds = getDirtyPagesFromLocalModel();

                    if (localPageIds.length > 0){
                        let localModelJson = localModel.toJSON();
                        settingsProvider.localModel.Parse(localModelJson);

                        settingsProvider.saveLocalModel(localPageIds);
                    }
                }
            }
        }
    }
}
