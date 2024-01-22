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

    color: Style.backgroundColor;

    property TreeItemModel settingsModel: TreeItemModel {
            onDataChanged: {
                console.log("settingsModel onDataChanged");
            }
      };

    property alias leftPanelWidth: mainPanelBackground.width;
    property alias mainWidth: root.width;
    property alias mainHeight: root.height;

    property string backgroundColor: Style.backgroundColor;
    property string fontColor: Style.textColor;

    property int fontSize: Style.fontSize_common;

    property bool modelIsDirty: false;

    signal modelChanged();

    onModelChanged: {
        root.modelIsDirty = true;
    }

    onSettingsModelChanged: {
        console.log("onSettingsModelChanged", settingsModel);

        if (root.settingsModel != null){
            console.log("connect");
            root.settingsModel.dataChanged.connect(root.modelChanged);

            root.updateGui();
        }
    }

    function clearModels(){
        root.settingsModel.Clear();
        mainPanelRepeater.model = 0;
        bodyPanelRepeater.model = 0;
    }

    function updateGui(){
        mainPanel.selectedIndex = 0;
        mainPanelRepeater.model = root.settingsModel;

        let parametersModel = root.settingsModel.GetData("Parameters", 0)
        bodyPanelRepeater.model = parametersModel
    }

    function getPageIndexByPageId(pageId){
        for (let i = 0; i < root.settingsModel.GetItemsCount(); i++){
            let id = root.settingsModel.GetData("Id", i);
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
        anchors.margins: Style.size_mainMargin;

        width: 150;
        height: parent.height;

        color: Style.baseColor;

        Component{
            id: defaultButtonDecorator;

            CommonButtonDecorator{
                width: mainPanelRepeater.width;
                height: 35;
            }
        }

        Column {
            id: mainPanel;

            anchors.fill: parent;
            anchors.topMargin: Style.size_mainMargin;

            property alias selectedIndex: mainPanelRepeater.currentIndex;

            spacing: Style.size_mainMargin;

            ListView {
                id: mainPanelRepeater;

                anchors.left: parent.left;
                anchors.leftMargin: Style.size_mainMargin;
                anchors.right: parent.right;
                anchors.rightMargin: Style.size_mainMargin;

                height: contentHeight;

                spacing: Style.size_mainMargin;

                delegate: ItemDelegate {
                    text: model.Name;

                    highlighted: mainPanelRepeater.currentIndex == model.index;

                    onClicked: {
                        mainPanelRepeater.currentIndex = model.index;
                        bodyPanelRepeater.model = model.Parameters;
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

        secondSize: 10;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_mainMargin;
        anchors.left: mainPanelBackground.right;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.size_mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_mainMargin;

        clip: true;

        contentWidth: width;
        contentHeight: bodyPanel.height;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyPanel;

            width: parent.width;

            spacing: 15;

            Repeater {
                id: bodyPanelRepeater;

                delegate: Item {
                    width: bodyPanel.width;

                    height: titleItem.height + rectSeparator.height + loader.height + 20;

                    visible: loader.item;

                    Text {
                        id: titleItem;

                        text: model.Name;

                        visible: loader.item;

                        font.pixelSize: root.fontSize;
                        color: root.fontColor;
                        font.family: Style.fontFamilyBold;
                    }

                    Rectangle {
                        id: rectSeparator;

                        anchors.top: titleItem.bottom;
                        anchors.topMargin: Style.size_smallMargin;

                        width: parent.width;
                        height: 1;

                        visible: loader.item;

                        color: Style.buttonBorderColor;
                    }

                    Loader {
                        id: loader;

                        anchors.top: rectSeparator.bottom;
                        anchors.topMargin: Style.size_mainMargin;

                        Component.onCompleted: {
                            if (model.Source && model.Source !== ""){
                                loader.source = model.Source;
                            }
                        }

                        onLoaded: {
                            console.log("Loader onLoaded", item.parameters);
                            if (loader.item.parameters !== undefined){
                                loader.item.parameters = model.Parameters;
                            }
                            if (loader.item.rootItem !== undefined){
                                loader.item.rootItem = root;
                            }
                        }
                    }
                }
            }
        }//Column
    }//Flickable
}
