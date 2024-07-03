import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

DecoratorBase {
    id: topCenterPanelDecorator;

//    Row {
//        anchors.verticalCenter: parent.verticalCenter;
//        anchors.horizontalCenter: parent.horizontalCenter;

//        height: 30;

//        spacing: Style.size_mainMargin;

//        Connections {
//            target: NavigationController;

//            function onCurrentIndexChanged(){
//                leftButton.enabled = NavigationController.currentIndex > 0;
//                rightButton.enabled = NavigationController.currentIndex < NavigationController.stack.length - 1;
//            }
//        }

//        Item {
//            height: parent.height;
//            width: leftButton.width + rightButton.width;

//            ToolButton {
//                id: leftButton;
//                anchors.left: parent.left;
//                width: 24;
//                height: parent.height;
//                iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
//                                    : "../../../" +  Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled);
//                enabled: false;
//                onClicked: {
//                    NavigationController.prev();
//                }
//            }

//            ToolButton {
//                id: rightButton;
//                anchors.left: leftButton.right;
//                width: 24;
//                height: parent.height;
//                iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
//                                    : "../../../" +  Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);
//                enabled: false;
//                onClicked: {
//                    NavigationController.next();
//                }
//            }
//        }

//        CustomTextField {
//            width: 350;
//            height: parent.height;
//            placeHolderText: qsTr("Enter text for search");
//            textFieldRightMargin: iconClear.width + 2 * margin;
//            onEditingFinished: {
//                Events.sendEvent("GlobalSearchActivated", text)
//            }

//            ToolButton {
//                id: iconClear;

//                z: 999;

//                anchors.verticalCenter: parent.verticalCenter;
//                anchors.right: parent.right;
//                anchors.rightMargin: Style.margin;

//                width: Style.buttonWidthSmall;
//                height: width;

//                visible: parent.text != "";

//                iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

//                onClicked: {
//                    console.log("iconClear onClicked")
//                    parent.text = "";
//                }
//            }
//        }
//    }
}

