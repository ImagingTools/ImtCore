import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3


Rectangle {
    id: container;

    color: Style.dialogBackgroundColor;
    radius: 3;

    property Item root;

    property TreeItemModel serverModel: TreeItemModel {}
    property TreeItemModel localModel: TreeItemModel {}

    property TreeItemModel commonModel: TreeItemModel {}

    property alias leftPanelWidth: mainPanelBackground.width;
    property alias mainWidth: mainRec.width;
    property alias mainHeight: mainRec.height;
    property alias topPanelSource: loaderTopPanel.source;

    property string backgroundColor: Style.backgroundColor;
    property string fontColor: Style.textColor;

    property int fontSize: Style.fontSize_common;

    property SettingsProvider settingsProvider: null;

    Component.onDestruction: {
        commonModel.modelChanged.disconnect(container.modelChanged);
    }

    onVisibleChanged: {
        console.log("onVisibleChanged settingsProvider", settingsProvider);
        if (visible){
            if (settingsProvider.localModel){
                let localModelJson = settingsProvider.localModel.toJSON();
                localModel.CreateFromJson(localModelJson);

                updateCommonModel(localModel);
            }

            if (settingsProvider.serverModel){
                let serverModelJson = settingsProvider.serverModel.toJSON();
                serverModel.CreateFromJson(serverModelJson);

                updateCommonModel(serverModel);
            }

            settingsProvider.rewriteModel(serverModel, localModel);

            localModel.modelChanged.connect(container.modelChanged);
            serverModel.modelChanged.connect(container.modelChanged);

            updateGui();

            console.log("setButtonState Apply false");
            buttonsDialog.setButtonState("Apply", false);
        }
        else{
            clearModels();
        }
    }

    function modelChanged(){
        console.log("Preference modelChanged");

        console.log("setButtonState Apply true");
        buttonsDialog.setButtonState("Apply", true);
    }

    function clearModels(){
        serverModel.Clear();
        localModel.Clear();
        commonModel.Clear();

        mainPanelRepeater.model = 0;
        bodyPanelRepeater.model = 0;
    }

    function updateGui(){
        mainPanelRepeater.model = commonModel;

        mainPanel.selectedIndex = 0;
        let parametersModel = commonModel.GetData("Parameters")
        bodyPanelRepeater.model = parametersModel
    }

    function updateCommonModel(externModel){
        if (externModel != undefined && externModel != null){
            for (let i = 0; i < externModel.GetItemsCount(); i++){
                let pageId = externModel.GetData("Id", i);
                let pageName = externModel.GetData("Name", i);
                let pageParameters = externModel.GetData("Parameters", i);

                let index = getPageIndexByPageId(pageId)

                if (index < 0){
                    index = commonModel.InsertNewItem();
                }

                commonModel.CopyItemDataFromModel(index, externModel, i);
            }
        }
    }

    function getPageIndexByPageId(pageId, model){
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

    function close(buttonId){
        container.visible = false;
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
                fileName["name"] = "ServerLog.txt";
            }

            remoteFileController.GetFile(id, fileName);
        }
    }

    function compare(model1, model2){
        let changeSet = []

        compareRecursive(model1, model2, changeSet);

        return changeSet;
    }

    function compareRecursive(model1, model2, changeSet){

        for (let i = 0; i < model1.GetItemsCount(); i++){
            let keys = model1.GetKeys(i);

            for (let j = 0; j < keys.length; j++){
                let key = keys[j];

                if (model2.ContainsKey(key)){
                    let model1Value = model1.GetData(key, i);
                    let model2Value = model2.GetData(key, i);

                    if(typeof model1Value === 'object' && typeof model2Value === 'object'){
                        compareRecursive(model1Value, model2Value, changeSet);
                    }
                    else{
                        if (model1Value != model2Value){
                            let changeObj = {}
                            changeSet[key] = model2Value;
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: mainRec;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        width: 450;
        height: 480;

        radius: container.radius;

        color: container.backgroundColor;

        Loader {
            id: loaderTopPanel;

            width: parent.width;
            source: "../../../qml/imtgui/AuxComponents/Dialogs/TopPanelDialog.qml";
            onLoaded:  {
                loaderTopPanel.item.title = "Preferences";
                loaderTopPanel.item.closeButtonClicked.connect(container.close);
            }
            onSourceChanged: {
                loaderTopPanel.item.title = "Preferences";
            }
        }

        Rectangle {
            id: mainPanelBackground;

            anchors.top: loaderTopPanel.bottom;
            anchors.left: parent.left;
            anchors.bottom: buttonsDialog.top;
            anchors.margins: 10;

            width: 150;
            height: parent.height;

            color: Style.baseColor;

            Component{
                id: defaultButtonDecorator;

                CommonButtonDecorator{
                    width: mainPanelRepeater.width;
                    height: 35;
                }
            }

            Column {
                id: mainPanel;

                anchors.fill: parent;
                anchors.topMargin: 10;

                property int selectedIndex: -1;

                spacing: 5;

                Repeater {
                    id: mainPanelRepeater;

                    delegate:
                        BaseButton{

                        id: buttonContainer;

                        anchors.left: parent.left;
                        anchors.leftMargin: 10;
                        anchors.right: parent.right;
                        anchors.rightMargin: 10;

                        decorator: Style.commonButtonDecorator !==undefined ? Style.commonButtonDecorator : defaultButtonDecorator;

                        selected: mainPanel.selectedIndex === model.index;
                        text: model.Name;

                        Component.onCompleted: {
                            if (model.index === 0){
                                clicked();
                            }
                        }

                        onClicked: {
                            if (mainPanel.selectedIndex !== model.index){
                                bodyPanelRepeater.model = model.Parameters;
                                mainPanel.selectedIndex = model.index;
                            }
                        }
                    }//delegate
                }

                onSelectedIndexChanged: {}
            }
        }

        Flickable {
            anchors.top: loaderTopPanel.bottom;
            anchors.topMargin: 10;
            anchors.left: mainPanelBackground.right;
            anchors.leftMargin: 10;
            anchors.right: parent.right;
            anchors.rightMargin: 10;
            anchors.bottom: buttonsDialog.top;
            anchors.bottomMargin: 10;

            clip: true;

            contentWidth: width;
            contentHeight: bodyPanel.height;

            boundsBehavior: Flickable.StopAtBounds;

            Column {
                id: bodyPanel;

                width: parent.width;

                spacing: 15;

                Repeater {
                    id: bodyPanelRepeater;

                    delegate: Item {
                        width: bodyPanel.width;

                        height: titleItem.height + rectSeparator.height + loader.height + 20;

                        Text {
                            id: titleItem;

                            text: model.Name;

                            font.pixelSize: container.fontSize;
                            color: container.fontColor;
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
                                loader.source = model.Source;
                            }

                            onLoaded: {
                                console.log("Loader onLoaded", item.parameters);
                                if (item.parameters !== undefined){
                                    item.parameters = model.Parameters;
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
                addButton({"Id":"Apply", "Name": qsTr("Apply"), "Enabled": false});
                addButton({"Id":"Close", "Name": qsTr("Close"), "Enabled": true});
            }

            onButtonClicked: {
                console.log("ButtonsDialog onButtonClicked", buttonId);

                if (buttonId == "Close"){
                    container.visible = false;
                }
                else if (buttonId == "Apply"){
                    if (!_.isEqual(JSON.stringify(container.serverModel), JSON.stringify(container.settingsProvider.serverModel))){
                        let serverModelJson = container.serverModel.toJSON();
                        container.settingsProvider.serverModel.CreateFromJson(serverModelJson);

                        container.settingsProvider.saveServerModel();
                    }

                    if (!_.isEqual(JSON.stringify(container.localModel), JSON.stringify(container.settingsProvider.localModel))){
                        let localModelJson = container.localModel.toJSON();
                        container.settingsProvider.localModel.CreateFromJson(localModelJson);

                        container.settingsProvider.saveLocalModel();
                    }

                    buttonsDialog.setButtonState("Apply", false);
                }
            }
        }
    }
}
