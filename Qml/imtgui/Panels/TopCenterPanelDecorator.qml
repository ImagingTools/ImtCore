import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0

DecoratorBase {
    id: topCenterPanelDecorator;

    property bool canSearch: false;
    visible: canSearch;

    Component.onCompleted: {
        Events.subscribeEvent("SearchVisible", topCenterPanelDecorator.setVisible);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SearchVisible", topCenterPanelDecorator.setVisible);
    }

    Connections {
        target: AuthorizationController;

		function onLoggedIn(){
            topCenterPanelDecorator.canSearch = true;
            topCenterPanelDecorator.visible = topCenterPanelDecorator.canSearch;
        }

		function onLoggedOut(){
            topCenterPanelDecorator.canSearch = false;
            topCenterPanelDecorator.visible = topCenterPanelDecorator.canSearch;
            tfc.text = "";
        }
    }

    function setVisible(visible){
        if (visible && !canSearch){
            topCenterPanelDecorator.visible = false;
            return;
        }

        topCenterPanelDecorator.visible = visible;
    }

    Row {
        anchors.verticalCenter: topCenterPanelDecorator.verticalCenter;
        anchors.horizontalCenter: topCenterPanelDecorator.horizontalCenter;

        height: 30;

        spacing: Style.sizeMainMargin;

        Connections {
            target: NavigationController;

            function onCurrentIndexChanged(){
                leftButton.enabled = NavigationController.hasPrev();
                rightButton.enabled = NavigationController.hasNext();
            }
        }

        Item {
            height: parent.height;
            width: leftButton.width + rightButton.width;

            ToolButton {
                id: leftButton;
                anchors.left: parent.left;
                width: 24;
                height: parent.height;
                iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
                                    : "../../../" +  Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled);
                enabled: false;
                decorator: Component {
                    ToolButtonDecorator {
                        icon.width: 16;
                    }
                }
                onClicked: {
                    NavigationController.prev();
                }
            }

            ToolButton {
                id: rightButton;
                anchors.left: leftButton.right;
                width: 24;
                height: parent.height;
                iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
                                    : "../../../" +  Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);
                enabled: false;
                decorator: Component {
                    ToolButtonDecorator {
                        icon.width: 16;
                    }
                }
                onClicked: {
                    NavigationController.next();
                }
            }
        }

        CustomTextField {
            id: tfc;
            width: 350;
            height: parent.height;
            placeHolderText: qsTr("Enter text for search");
            textFieldRightMargin: iconClear.width + 2 * margin;
            radius: 4;

            onTextChanged: {
                timer.restart();
            }

            Timer {
                id: timer;
                interval: 500;
                onTriggered: {
                    Events.sendEvent("GlobalSearchActivated", tfc.text)
                }
            }

            ToolButton {
                id: iconClear;

                z: 999;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: Style.margin;

                width: Style.buttonWidthSmall;
                height: width;

                visible: parent.text != "";

                iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
                decorator: Component {
                    ToolButtonDecorator {
                        color: "transparent";
                        icon.width: 16;
                    }
                }

                onClicked: {
                    parent.text = "";
                }
            }
        }
    }
}

