import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: tabPanelContainer;

    height: 40;
    width: 1000;

//    color: Style.backgroundColor;
    color: Style.tabPanelBackgroundColor;

    property int selectedIndex: 0;
    property int count: 0;

    property string firstElementImageSource;
    property bool isCloseEnable: true;

    property alias model: list.model;

    signal closeItem(int index);
    signal rightClicked();
    signal leftClicked();

    function viewTabInListView(index) {
        list.positionViewAtIndex(index, ListView.Contain);
    }

    onSelectedIndexChanged: {
        viewTabInListView(selectedIndex);
    }

    ListView {
        id: list;

        width: externButtons.visible ? parent.width - externButtons.width :  parent.width;
        height: parent.height;

        clip: true;
        orientation: ListView.Horizontal;

        boundsBehavior: Flickable.StopAtBounds;

        spacing: 0;

        delegate: TabDelegate {
            height: list.height;

            selected: model.index === tabPanelContainer.selectedIndex;
            selectedIndex: tabPanelContainer.selectedIndex;
            firstElement: model.index === 0;
            lastElement: model.index === list.count - 1;
            firstElementImageSource: tabPanelContainer.firstElementImageSource;
            text: model.Title ? model.Title : "";
            isCloseEnable: tabPanelContainer.isCloseEnable;
            listView: list;

            onClicked: {
                tabPanelContainer.selectedIndex = model.index;
            }

            onCloseSignal: {
                tabPanelContainer.closeItem(model.index);
            }
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

        Button {
            id: leftButton;

            anchors.left: externButtons.left;
            anchors.verticalCenter: externButtons.verticalCenter;

            width: externButtons.width / 2;
            height: externButtons.height;

            enabled: tabPanelContainer.selectedIndex > 0;

            iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
                                : "../../../" +  Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled);

            onClicked: {
                tabPanelContainer.leftClicked();
            }
        }

        Button {
            id: rightButton;

            anchors.right: externButtons.right;
            anchors.verticalCenter: externButtons.verticalCenter;

            width: externButtons.width / 2;
            height: externButtons.height;

            enabled: tabPanelContainer.selectedIndex < list.count - 1;

            iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
                                : "../../../" +  Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);

            onClicked: {
                tabPanelContainer.rightClicked();
            }
        }
    }
}
