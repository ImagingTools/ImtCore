import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: comboBoxContainerGql;

    width: 90;
    height: 30;

    property var model;
    property var properties;
    property var gettedParams;
    property var filterIdsModel;

    property color borderColor: comboBoxContainerGql.focus ? Style.iconColorOnSelected : Style.alternateBaseColor;

    property color backgroundColor: Style.baseColor;

    property string currentText;

    property bool textCentered: false;
    property bool menuVisible: false;
    property bool isColor: false;
    property bool backVisible: true;
    property bool hiddenBackground: true;
    property bool canClose: true;
    property bool complexModel: false;
    property bool openST: false;

    property int radius: 5;
    property int currentIndex: -1;
    property int offset: 0;
    property int count: 15;
    property int delegateRadius: 0;
    property int itemHeight: 26;
    property int textSize: Style.fontSize_common;
    property int dialogsCountPrev: 1000;
    property int dialogsCount: modalDialogManager.count;

    property string commandId: "";
    property string filterName: "Name";
    property string filterText: "";
    property Component delegate: PopupMenuDelegate{width: comboBoxContainerGql.width; height: comboBoxContainerGql.itemHeight;};
    property alias popupComp: popupMenu;
    property alias gradient: cbMainRect.gradient;

    property alias image: cbArrowIcon;


    signal setCurrentText(var modelll, int index);

    onSetCurrentText: {
        if(!comboBoxContainerGql.complexModel){
            comboBoxContainerGql.currentText = modelll.GetData("Name",index);
        }
    }


    signal clicked();
    signal finished(string commandId, int index);


    onModelChanged: {
        if (comboBoxContainerGql.currentIndex > -1){
            comboBoxContainerGql.currentText = popup.model.GetData("Name");
        }
    }

    onDialogsCountChanged: {

        comboBoxContainerGql.openST = comboBoxContainerGql.dialogsCount > comboBoxContainerGql.dialogsCountPrev;
        if(!openST && dialogsCountPrev < 1000){
            dialogsCountPrev = 1000;
        }
    }


    Component {
        id: popupMenu;
        PopupMenuDialogGql {
            id: popup;
            offset: comboBoxContainerGql.offset;
            count: comboBoxContainerGql.count;
            commandId: comboBoxContainerGql.commandId;
            filterName: comboBoxContainerGql.filterName;
            delegate: comboBoxContainerGql.delegate;
            properties: comboBoxContainerGql.properties;
            gettedParams: comboBoxContainerGql.gettedParams;
            filterIdsModel: comboBoxContainerGql.filterIdsModel;
            delegateRadius: comboBoxContainerGql.delegateRadius;
            hiddenBackground: comboBoxContainerGql.hiddenBackground;
            itemHeight: comboBoxContainerGql.itemHeight;
            textSize: comboBoxContainerGql.textSize;
            canClose: comboBoxContainerGql.canClose;
//            function getCurrentText(index){
//                return popup.model.GetData("Name",index);
//            }
//            Connections{
//                target: comboBoxContainerGql;
//                onFinished: popup.finished(commandId, index)
//            }

            Component.onCompleted: {
                comboBoxContainerGql.finished.connect(popup.finished);
            }

            onFilterTextChanged: {
                comboBoxContainerGql.filterText = popup.filterText;
                comboBoxContainerGql.currentIndex = -1;
                comboBoxContainerGql.currentText = popup.filterText;
            }
            onFinished: {
                //console.log("__________FINISHED______________")
                console.log(index)
                console.log(commandId)
                if (index > -1){
                    for (var item = 0; item < gettedParams.GetItemsCount(); item++){
                        let param = comboBoxContainerGql.gettedParams.GetData("Name",  item);
                        let value = popup.model.GetData(param, index);
                        console.log(param, " = ", value);
                        comboBoxContainerGql.gettedParams.SetData("Value", value, item);
                    }
                }
                comboBoxContainerGql.currentIndex = index;
                //comboBoxContainerGql.currentText = popup.model.GetData("Name", index);
                comboBoxContainerGql.setCurrentText(popup.model,index)
                if (comboBoxContainerGql.currentText == ""){
                    comboBoxContainerGql.currentText = popup.filterText;
                }

            }
            onPropertiesChanged: {
                comboBoxContainerGql.currentIndex = -1;
                comboBoxContainerGql.currentText = "";
            }


        }
    }

    Component.onCompleted: {
        if (comboBoxContainerGql.textCentered){
            cbTitleTxt.anchors.horizontalCenter = cbMainRect.horizontalCenter;
        }
        else {
            cbTitleTxt.anchors.left = cbMainRect.left;
            cbTitleTxt.anchors.leftMargin = 10;
        }
    }

    function openPopupMenu(){
        comboBoxContainerGql.dialogsCountPrev = modalDialogManager.count;
        var point = comboBoxContainerGql.mapToItem(thumbnailDecoratorContainer, 0, 0);
        modalDialogManager.openDialog(popupMenu, { "x":     point.x,
                                                   "y":     point.y,
                                                   "model": comboBoxContainerGql.model,
                                                   "width": comboBoxContainerGql.width,
                                                   "countVisibleItem": 5 });
    }

    Rectangle {
        id: cbMainRect;

        anchors.fill: parent;

        border.color: comboBoxContainerGql.borderColor;
        border.width: !comboBoxContainerGql.backVisible ? 0 :1;

        radius: comboBoxContainerGql.radius;
        color: comboBoxContainerGql.backgroundColor;

        gradient: Gradient {
            GradientStop { position: 0.0; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient4; }
        }



        Text {
            id: cbTitleTxt;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 10;

            color: Style.textColor;
            text: comboBoxContainerGql.currentText;
            font.family: Style.fontFamily;
            font.pixelSize: comboBoxContainerGql.textSize;
            visible: comboBoxContainerGql.backVisible;
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
                comboBoxContainerGql.clicked();


            }
        }
    }
}
