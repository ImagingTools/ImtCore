import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: tabPanelContainer;

    width: list.contentWidth + externButtons.width;
    height: 40;

    color: Style.tabPanelBackgroundColor;

    property int selectedIndex: 0;
    property int count: 0;
    property alias spacing: list.spacing;

    property string firstElementImageSource;
    property string displayRoleId: "Title";
    property bool isCloseEnable: true;

    property alias model: list.model;
    property Component tabDelegateDecorator: Style.tabPanelDecorator;

    signal closeItem(int index);
    signal rightClicked();
    signal leftClicked();

    Component.onCompleted: {
        Events.subscribeEvent("AppSizeChanged", appSizeChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("AppSizeChanged", appSizeChanged);
    }

    function viewTabInListView(index) {
        list.positionViewAtIndex(index, ListView.Contain);
    }

    function appSizeChanged(){
        viewTabInListView(selectedIndex);
    }

    onSelectedIndexChanged: {
        viewTabInListView(selectedIndex);
    }

    ListView {
        id: list;

        anchors.left: parent.left;
        width: externButtons.visible ? tabPanelContainer.width - externButtons.width : contentWidth

        height: parent.height;

        clip: true;

        orientation: ListView.Horizontal;
        boundsBehavior: Flickable.StopAtBounds;
        interactive: false;

        spacing: 0;

        onContentWidthChanged: {
            tabPanelContainer.viewTabInListView(tabPanelContainer.selectedIndex);
        }

        delegate: TabDelegate {
            height: list.height;

            selected: model.index === tabPanelContainer.selectedIndex;
            selectedIndex: tabPanelContainer.selectedIndex;
            firstElement: model.index === 0;
            lastElement: model.index === list.count - 1;
            firstElementImageSource: tabPanelContainer.firstElementImageSource;
            text: model[tabPanelContainer.displayRoleId] ? model[tabPanelContainer.displayRoleId] : "";
            isCloseEnable: tabPanelContainer.isCloseEnable;
            listView: list;
            decorator: tabPanelContainer.tabDelegateDecorator;

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

        width: visible ? height : 0;
        height: parent.height;

        color: Style.backgroundColor;

        visible: list.contentWidth > tabPanelContainer.width;

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
