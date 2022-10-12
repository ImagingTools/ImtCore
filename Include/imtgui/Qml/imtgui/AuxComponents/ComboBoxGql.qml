import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: comboBoxContainer;

    width: 90;
    height: 30;

    property var model;
    property var properties;
    property var gettedParams;
    property var filterIdsModel;

    property color borderColor: comboBoxContainer.focus ? Style.iconColorOnSelected : Style.alternateBaseColor;

    property color backgroundColor: Style.baseColor;

    property string currentText;

    property bool textCentered: false;
    property bool menuVisible: false;
    property bool isColor: false;
    property bool backVisible: true;
    property bool hiddenBackground: true;

    property int radius: 5;
    property int currentIndex: -1;
    property int offset: 0;
    property int count: 15;
    property int delegateRadius: 0;
    property int itemHeight: 26;
    property int textSize: Style.fontSize_common;
    property string commandId: "";
    property string filterName: "Name";
    property string filterText: "";
    property Component delegate: PopupMenuDelegate{width: comboBoxContainer.width; height: comboBoxContainer.height;};
    property alias popupComp: popupMenu;
    property alias gradient: cbMainRect.gradient;


    signal clicked();
    signal finished(string commandId, int index);


    onModelChanged: {
        if (comboBoxContainer.currentIndex > -1){
            comboBoxContainer.currentText = popup.model.GetData("Name");
        }
    }

    Component {
        id: popupMenu;
        PopupMenuDialogGql {
            id: popup;
            offset: comboBoxContainer.offset;
            count: comboBoxContainer.count;
            commandId: comboBoxContainer.commandId;
            filterName: comboBoxContainer.filterName;
            delegate: comboBoxContainer.delegate;
            properties: comboBoxContainer.properties;
            gettedParams: comboBoxContainer.gettedParams;
            filterIdsModel: comboBoxContainer.filterIdsModel;
            delegateRadius: comboBoxContainer.delegateRadius;
            hiddenBackground: comboBoxContainer.hiddenBackground;
            itemHeight: comboBoxContainer.itemHeight;
            textSize: comboBoxContainer.textSize;
            Connections{
                target: comboBoxContainer;
                onFinished: popup.finished(commandId, index)
            }
            onFilterTextChanged: {
                comboBoxContainer.filterText = popup.filterText;
                comboBoxContainer.currentIndex = -1;
                comboBoxContainer.currentText = popup.filterText;
            }
            onFinished: {
                console.log("__________FINISHED______________")
                console.log(index)
                console.log(commandId)
                if (index > -1){
                    for (var item = 0; item < gettedParams.GetItemsCount(); item++){
                        let param = comboBoxContainer.gettedParams.GetData("Name",  item);
                        let value = popup.model.GetData(param, index);
                        console.log(param, " = ", value);
                        comboBoxContainer.gettedParams.SetData("Value", value, item);
                    }
                }
                comboBoxContainer.currentIndex = index;
                comboBoxContainer.currentText = popup.model.GetData("Name", index);
                if (comboBoxContainer.currentText == ""){
                    comboBoxContainer.currentText = popup.filterText;
                }

            }
            onPropertiesChanged: {
                comboBoxContainer.currentIndex = -1;
                comboBoxContainer.currentText = "";
            }


        }
    }

    Component.onCompleted: {
        if (comboBoxContainer.textCentered){
            cbTitleTxt.anchors.horizontalCenter = cbMainRect.horizontalCenter;
        }
        else {
            cbTitleTxt.anchors.left = cbMainRect.left;
            cbTitleTxt.anchors.leftMargin = 10;
        }
    }

    function openPopupMenu(){
        var point = comboBoxContainer.mapToItem(thumbnailDecoratorContainer, 0, 0);
        modalDialogManager.openDialog(popupMenu, { "x":     point.x,
                                                   "y":     point.y,
                                                   "model": comboBoxContainer.model,
                                                   "width": comboBoxContainer.width,
                                                   "countVisibleItem": 5 });
    }

    Rectangle {
        id: cbMainRect;

        anchors.fill: parent;

        border.color: comboBoxContainer.borderColor;
        border.width: !comboBoxContainer.backVisible ? 0 :1;

        radius: comboBoxContainer.radius;
        color: comboBoxContainer.backgroundColor;

        gradient: Gradient {
            GradientStop { position: 0.0; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: comboBoxContainer.isColor ? cbMainRect.color : Style.imagingToolsGradient4; }
        }



        Text {
            id: cbTitleTxt;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            color: Style.textColor;
            text: comboBoxContainer.currentText;
            font.family: Style.fontFamily;
            font.pixelSize: comboBoxContainer.textSize;
            visible: comboBoxContainer.backVisible;
        }

        Image {
            id: cbArrowIcon;

            anchors.right: cbMainRect.right;
            anchors.verticalCenter: cbMainRect.verticalCenter;
            anchors.rightMargin: 5;

            width: 12;
            height: 10;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

            sourceSize.width: width;
            sourceSize.height: height;
        }

        MouseArea {
            id: cbMouseArea;

            anchors.fill: parent;

            cursorShape: Qt.PointingHandCursor;

            onClicked: {
                console.log("ComboBox clicked !");
                openPopupMenu();
                comboBoxContainer.clicked();


            }
        }
    }
}
