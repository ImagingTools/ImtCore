import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: tabPanelContainer;

    height: 40;
    width: 1000;

    color: Style.theme === "Dark" ? Style.baseColor: Style.backgroundColor;

    property int selectedIndex: 0;
    property int count: 0;

    property string firstElementImageSource;

//    property var headersArray: [];

    property alias model: list.model;

    signal closeItem(int index);
    signal rightClicked();
    signal leftClicked();

    function viewTabInListView(index) {
        list.positionViewAtIndex(index, ListView.Contain);
    }

    ListView{
        id: list;

        width: externButtons.visible ? parent.width - externButtons.width :  parent.width;
        height: parent.height;

        clip: true;
        orientation: ListView.Horizontal;

        spacing: 0;

        boundsBehavior: Flickable.StopAtBounds;

        delegate: TabDelegate {
            height: list.height;

            selected: model.index === tabPanelContainer.selectedIndex;
            firstElement: model.index === 0;
            lastElement: model.index === list.count - 1;
            firstElementImageSource: tabPanelContainer.firstElementImageSource;
            text: model.Title;

            decoratorSource: Style.tabPanelDecoratorPath;

            onClicked: {
                console.log("TabDelegate onClicked", list.model)
                tabPanelContainer.selectedIndex = model.index;
            }

            onCloseSignal: {
                tabPanelContainer.closeItem(model.index);
            }
        }
    }

    Rectangle {
        anchors.right: externButtons.left;

        height: parent.height;
        width: parent.height;

        visible: externButtons.visible;
        rotation: -90;

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#00ffffff"; }
            GradientStop { position: 0.5; color: "#00ffffff"; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient2; }
        }
    }

    Rectangle {
        id: externButtons;

        anchors.right: parent.right;

        height: parent.height;
        width: height;

        color: Style.backgroundColor;

        visible: list.contentWidth > tabPanelContainer.width;

        MouseArea {
            anchors.fill: parent;
        }

        AuxButton {
            id: leftButton;

            anchors.left: externButtons.left;
            anchors.verticalCenter: externButtons.verticalCenter;

            width: externButtons.width / 2;
            height: externButtons.height;


            hasIcon: true;
            hasText: false;

            iconHeight: 10;
            iconWidth: 10;
            iconSource: "../../../" + "Icons/" + Style.theme + "/Left_On_Normal.svg";
            borderColor: Style.buttonColor;

            onClicked: {
                tabPanelContainer.leftClicked();
            }
        }

        AuxButton {
            id: rightButton;

            anchors.right: externButtons.right;
            anchors.verticalCenter: externButtons.verticalCenter;

            width: externButtons.width / 2;
            height: externButtons.height;

            hasIcon: true;
            hasText: false;
            iconHeight: 10;
            iconWidth: 10;
            iconSource: "../../../" + "Icons/" + Style.theme + "/Right_On_Normal.svg";
            borderColor: Style.buttonColor;

            onClicked: {
                tabPanelContainer.rightClicked();
            }
        }
    }
}
