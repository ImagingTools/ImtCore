import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3

Rectangle {
    id: container;

    color: "transparent";

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

    property alias applicationInfoProvider: aboutApplicationProvider.applicationInfoProvider;

    property int pageWidth: 450;
    property int pageHeight: 480;

    signal saved();
    signal closed();

    Rectangle{
        anchors.fill: parent;
        color: Style.dialogBackgroundColor;
        opacity: 0.5;
    }

    PageAboutProvider {
        id: aboutApplicationProvider;
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        container.commonModel.dataChanged.disconnect(container.modelChanged);
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    onApplicationInfoProviderChanged: {
        if (container.applicationInfoProvider != null){

        }
    }

    onVisibleChanged: {
        console.log("onVisibleChanged settingsProvider", container.settingsProvider);
        if (visible){
            //console.log("settingsProvider.localModel", settingsProvider.localModel);
            if (container.settingsProvider && container.settingsProvider.localModel){
                let localModelJson = container.settingsProvider.localModel.toJSON();

                console.log("localModelJson", localModelJson);
                container.localModel.CreateFromJson(localModelJson);

//                updateCommonModel(localModel);
            }

            updateCommonModel(container.localModel);

           // console.log("settingsProvider.serverModel", container.settingsProvider.serverModel);
            if (container.settingsProvider && container.settingsProvider.serverModel){
                let serverModelJson = container.settingsProvider.serverModel.toJSON();
                container.serverModel.CreateFromJson(serverModelJson);

//                updateCommonModel(serverModel);
            }

            updateCommonModel(container.serverModel);

            updateCommonModel(aboutApplicationProvider.pageModel);

            if (container.settingsProvider){
                container.settingsProvider.rewriteModel(container.serverModel, container.localModel);
            }

            container.localModel.dataChanged.connect(container.modelChanged);
            container.serverModel.dataChanged.connect(container.modelChanged);

            updateGui();

            buttonsDialog.setButtonState("Apply", false);
        }
        else{
            clearModels();
        }
    }

    function modelChanged(){
        console.log("Preference modelChanged");
        buttonsDialog.setButtonState("Apply", true);
    }

    function clearModels(){
        container.serverModel.Clear();
        container.localModel.Clear();
        container.commonModel.Clear();

        mainPanelRepeater.model = 0;
        bodyPanelRepeater.model = 0;
    }

    function updateGui(){
        mainPanelRepeater.model = container.commonModel;

        mainPanel.selectedIndex = 0;
        let parametersModel = container.commonModel.GetData("Parameters")
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
                    index = container.commonModel.InsertNewItem();
                }

                container.commonModel.SetData("Id", pageId, index);
                container.commonModel.SetData("Name", pageName, index);

                if(pageParameters != null){
                    container.commonModel.SetData("Parameters", pageParameters, index);
                }
            }
        }
    }

    function getPageIndexByPageId(pageId){
        for (let i = 0; i < container.commonModel.GetItemsCount(); i++){
            let id = container.commonModel.GetData("Id", i);
            if (id === pageId){
                return i;
            }
        }

        return -1;
    }

    function close(buttonId){
        container.visible = false;
    }

    MouseArea {
        anchors.fill: parent;

        acceptedButtons: Qt.AllButtons
        hoverEnabled: true;

        onWheel: {
            wheel.accepted = true;
        }
    }

    function onLocalizationChanged(language){
        console.log("Preference onLocalizationChanged", language);
        loaderTopPanel.item.title = qsTr("Preferences");

        for (let i = 0; i < buttonsDialog.buttons.count; i++){
            let id = buttonsDialog.buttons.get(i).Id;
            if (id === "Apply"){
                buttonsDialog.buttons.setProperty(i, "Name", qsTr("Apply"));
            }
            else if (id === "Close"){
                buttonsDialog.buttons.setProperty(i, "Name", qsTr("Close"));
            }
        }
    }

    Rectangle {
        id: mainRec;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        width: container.pageWidth;
        height: container.pageHeight;

        radius: container.radius;

        color: container.backgroundColor;

        Loader {
            id: loaderTopPanel;

            width: parent.width;
            source: "../../../qml/imtgui/AuxComponents/Dialogs/TopPanelDialog.qml";

//            sourceComponent: Component {
//                TopPanelDialog {
//                    title: qsTr("Preferences");
//                }
//            }

            onLoaded:  {
                loaderTopPanel.item.title = qsTr("Preferences");
                loaderTopPanel.item.closeButtonClicked.connect(container.close);
            }
            onSourceChanged: {
                loaderTopPanel.item.title = qsTr("Preferences");
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
                        width: mainPanel.width - 2*anchors.leftMargin;

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

        CustomScrollbar {
            id: scrollbar;

            z: 100;

            anchors.right: parent.right;
            anchors.bottom: buttonsDialog.top;

            backgroundColor: Style.baseColor;

            secondSize: 10;
            targetItem: flickable;
        }

        Flickable {
            id: flickable;

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
                            font.family: Style.fontFamilyBold;
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
                                if (loader.item.parameters !== undefined){
                                    loader.item.parameters = model.Parameters;
                                }
                                if (loader.item.rootItem !== undefined){
                                    loader.item.rootItem = container;
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

                    container.closed();
                }
                else if (buttonId == "Apply"){
                    if (container.settingsProvider && container.settingsProvider.serverModel != null){
                        if (!container.serverModel.IsEqualWithModel(container.settingsProvider.serverModel)){
                            container.settingsProvider.serverModel.Copy(container.serverModel);
                            container.settingsProvider.saveServerModel();
                        }
                    }

                    if (container.settingsProvider && container.settingsProvider.localModel != null){
                        let isEqual = container.localModel.IsEqualWithModel(container.settingsProvider.localModel);
                        console.log("isEqual", isEqual);

                        if (!isEqual){
                            container.settingsProvider.localModel.Copy(container.localModel);
                            container.settingsProvider.saveLocalModel();
                        }
                    }

                    buttonsDialog.setButtonState("Apply", false);

                    container.saved();
                }
            }
        }
    }
}
