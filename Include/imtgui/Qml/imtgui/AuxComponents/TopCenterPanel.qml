import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topCenterPanel;

    height: 75;
    width: 100;

    clip: true;

    property Item topPanel;

    property alias lvButtons: topCenterLvButtons;

    property string mode: "Buttons";
    property string customComponent: "Buttons";
    property string decoratorSource;

    onDecoratorSourceChanged: {
        if (topCenterPanel.decoratorSource){
            customLoader.source = topCenterPanel.decoratorSource;
            topCenterPanel.showCustomLoader();
            mainItem.anchors.leftMargin = 0;
            mainItem.anchors.rightMargin = 0;
        }
    }

    onWidthChanged: {
        if (topCenterPanel.decoratorSource){
            customLoader.item.width = topCenterPanel.width;
        }
    }

    function setModelButtons(modelLocal){
        console.log("topCenterPanel setModelButtons", modelLocal);
        topCenterLvButtons.model = modelLocal;
    }

    function showCustomLoader(){
        customLoader.visible = true;
        topCenterLvButtons.visible = false;
    }

    function showButtons(){
        customLoader.visible = false;
        topCenterLvButtons.visible = true;
    }

    function getCommandModeById(commandId){
        console.log("topCenterPanel getCommandModeById", commandId);

        for (let i = 0; i < topCenterLvButtons.model.GetItemsCount(); i++){
            let m_commandId = topCenterLvButtons.model.GetData("Id", i);
            if (m_commandId == commandId){
                let m_mode = topCenterLvButtons.model.GetData("Mode", i);
                return m_mode;
            }
        }

        return null;
    }

    Item {
        id: mainItem;

        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        height: parent.height;

        Loader {
            id: customLoader;

            visible: false;
            onItemChanged: {
                if (customLoader.item){
                    customLoader.item.width = parent.width;
                    customLoader.item.height = parent.height;
                }
            }
        }

        ListView {
            id: topCenterLvButtons;

            anchors.horizontalCenter: parent.horizontalCenter;

            height: parent.height;
            width: contentWidth > parent.width ? parent.width : contentWidth;

            clip: true;
            orientation: ListView.Horizontal;
            boundsBehavior: Flickable.StopAtBounds;

            visible: topCenterPanel.mode == "Buttons";

            Component.onCompleted: {
                console.log("topCenterLvButtons onCompleted")
                console.log("topCenterPanel.mode", topCenterPanel.mode)
            }

            delegate: TopButton {
                id: topButtonDelegate;

                text: model["Name"];
                isEmpty: model["Name"] === "";
                imageSource: "../../../" + "Icons/" + Style.theme + "/" + model["Icon"] + "_" + "Off" + "_" + model["Mode"] + ".svg";
                fontName: Style.fontFamily;
                checkable: model["Mode"] == "Normal";
                visible: x + width <= topCenterLvButtons.width;

                Component.onCompleted: {
                    console.log("topButtonDelegate onCompleted")
                }

                onCheckableChanged: {
                    console.log("TopButton onCheckableChanged", checkable);
                    let parameters = {};
                    parameters["Id"] = model.Id;
                    parameters["Mode"] = model.Mode;
                    parameters["Checkable"] = checkable;

                    Events.sendEvent("CommandUpdated", parameters);
                }

                onVisibleChanged: {
                    console.log("TopPanel onVisibleChanged",topButtonDelegate.text, topButtonDelegate.visible);
                }

                onClicked: {
                    console.log("TopPanel menuActivited", model.Id)
                    topPanel.menuActivatedSignal(model.Id);
                }
            }
        }
    }
}

