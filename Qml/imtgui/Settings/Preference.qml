import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: root;

    anchors.horizontalCenter: parent.horizontalCenter;
    anchors.verticalCenter: parent.verticalCenter;

    width: 450;
    height: 480;

    color: Style.backgroundColor2;

    property TreeItemModel settingsModel: TreeItemModel {
    };

    property alias leftPanelWidth: mainPanelBackground.width;
    property alias mainWidth: root.width;
    property alias mainHeight: root.height;

    property string backgroundColor: Style.backgroundColor;
    property string fontColor: Style.textColor;

    property int fontSize: Style.fontSizeXSmall;

    property bool modelIsDirty: false;

    signal modelChanged();

    onModelChanged: {
        root.modelIsDirty = true;
    }

    onSettingsModelChanged: {
        if (root.settingsModel != null){
            root.settingsModel.modelChanged.connect(root.modelChanged);

            root.updateGui();
        }
    }

    function clearModels(){
        root.settingsModel.clear();
        bodyPanel.model = 0;
    }

    function updateGui(){
        mainPanel.selectedIndex = 0;

        let parametersModel = root.settingsModel.getData("Parameters", 0)
        bodyPanel.model = parametersModel
    }

    function getPageIndexByPageId(pageId){
        for (let i = 0; i < root.settingsModel.getItemsCount(); i++){
            let id = root.settingsModel.getData("Id", i);
            if (id === pageId){
                return i;
            }
        }

        return -1;
    }

    Rectangle {
        id: mainPanelBackground;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.margins: Style.sizeMainMargin;

        width: 150;
        height: parent.height;

        color: Style.baseColor;

        Column {
            id: mainPanel;

            anchors.fill: parent;
            anchors.topMargin: Style.sizeMainMargin;

            property alias selectedIndex: mainPanelRepeater.currentIndex;

            spacing: Style.sizeMainMargin;

            ListView {
                id: mainPanelRepeater;

                anchors.left: parent.left;
                anchors.leftMargin: Style.sizeMainMargin;
                anchors.right: parent.right;
                anchors.rightMargin: Style.sizeMainMargin;

                height: contentHeight;

                spacing: Style.sizeMainMargin;
                model: root.settingsModel;

                boundsBehavior: Flickable.StopAtBounds;

                delegate: ItemDelegate {
                    text: model.Name;
                    highlighted: mainPanelRepeater.currentIndex == model.index;
                    onClicked: {
                        mainPanelRepeater.currentIndex = model.index;
                        bodyPanel.model = model.Parameters;
                    }
                }
            }

            onSelectedIndexChanged: {}
        }
    }

    CustomScrollbar {
        id: scrollbar;
        z: 100;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.top: parent.top;
        secondSize: Style.sizeMainMargin;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;
        anchors.top: parent.top;
        anchors.topMargin: Style.sizeLargeMargin;
        anchors.left: mainPanelBackground.right;
        anchors.leftMargin: Style.sizeLargeMargin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.sizeLargeMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.sizeLargeMargin;
        clip: true;
        contentWidth: width;
        contentHeight: bodyPanel.height;
        boundsBehavior: Flickable.StopAtBounds;

        ComposedParamsGui {
            id: bodyPanel;
            width: parent.width;
        }
    }//Flickable
}
