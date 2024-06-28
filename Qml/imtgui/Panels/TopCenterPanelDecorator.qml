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
//            }

//            ToolButton {
//                id: rightButton;

//                anchors.left: leftButton.right;

//                width: 24;
//                height: parent.height;

//                iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
//                                    : "../../../" +  Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);
//            }
//        }

//        CustomTextField {
//            width: 350;
//            height: parent.height;

//            placeHolderText: qsTr("Enter text for search");
//        }
//    }


//    CommandsDecorator {
//        id: commandsDecorator;

//        anchors.fill: parent;
//    }
}

