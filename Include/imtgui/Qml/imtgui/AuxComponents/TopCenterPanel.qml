import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: topCenterPanel;

    height: 55;
    width: 100;
    clip: true;

    property Item topPanel;

    property alias lvButtons: lvButtons;

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

    function showCustomLoader(){
        customLoader.visible = true;
        lvButtons.visible = false;
    }

    function showButtons(){
        customLoader.visible = false;
        lvButtons.visible = true;
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
            id: lvButtons;

            anchors.horizontalCenter: parent.horizontalCenter;

            height: parent.height;
            width: contentWidth > parent.width ? parent.width : contentWidth;

            clip: true;
            orientation: ListView.Horizontal;
            boundsBehavior: Flickable.StopAtBounds;

            visible: topCenterPanel.mode == "Buttons";

            delegate: TopButton {
                id: topButtonDelegate;

                text: model[CommandEnum.NAME];
                isEmpty: model[CommandEnum.NAME] === "";
                imageSource: "../../../" + "Icons/" + Style.theme + "/" + model[CommandEnum.ICON] + "_" + "Off" + "_" + model["Mode"] + ".svg";
                fontName: Style.fontFamily;
                checkable: model["Mode"] === "Normal";
                visible: x + width <= lvButtons.width;

                onVisibleChanged: {
                    console.log("TopPanel onVisibleChanged",topButtonDelegate.text, topButtonDelegate.visible);
//                    if (!topButtonDelegate.visible && lvButtons.width !== 0) {
//                        for (var i = 0; i < modelButtons.count; i++) {
//                            if (modelButtons.get(i).id !== "" && modelButtons.get(i).id === model[CommandEnum.ID]) {
//                               return;
//                            }
//                        }
//                        modelButtons.append({"id": model[CommandEnum.ID], "imageSource": topButtonDelegate.imageSource, "name": topButtonDelegate.text, "mode": model["Mode"]});
//                    }
//                    else if (topButtonDelegate.visible && lvButtons.width !== 0) {
//                        var j;
//                        for (var i = 0; i < modelButtons.count; i++) {
//                            if (modelButtons.get(i).id === model[CommandEnum.ID]) {
//                                modelButtons.remove(i)
//                                j = i;
//                                break;
//                            }
//                        }
//                        for (var i = j; i < modelButtons.count; i++) {
//                            modelButtons.remove(i)
//                        }
//                    }
                }

                onClicked: {
                    console.log("TopPanel menuActivited", model.Id)
                    topPanel.menuActivatedSignal(model.Id);
                }
            }
        }
    }
}

