import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: menuPanel;

    color: Style.backgroundColor;

    radius: 7;

    clip: true;

    property string textColor: Style.textColor;
    property string fontName: "Helvetica";
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    property string firstElementImageSources: [];

    property int activePageIndex: -1;
    property int pagesCount: lvPages.count;

    property TreeItemModel model: TreeItemModel {};

    property int spacing: 0;

    signal activePageChanged;

    function clearModels(){
        model.Clear();
        lvPages.model = 0;
        activePageIndex = -1;
    }

    onActivePageIndexChanged: {
        lvPages.currentIndex = menuPanel.activePageIndex;
    }

    onModelChanged: {
        console.log("MenuPanel onModelChanged", model);

        lvPages.model = model;
    }

    ListView {
        id: lvPages;

        anchors.fill: parent;

        boundsBehavior: Flickable.StopAtBounds;

        delegate: MenuPanelButton {
            Component.onCompleted: {
                if (model.index == 0){
                    clicked();
                }
            }

            text:  model["Name"];
            textColor: Style.textColor;
            fontName: menuPanel.fontName;
            imageSource: (highlighted || selected) ? "../../../" + "Icons/" + Style.theme + "/" + model["Icon"] + "_On_Selected.svg":
                                                     "../../../" + "Icons/" + Style.theme + "/" + model["Icon"] + "_On_Normal.svg";
            decoratorSource : Style.menuButtonDecoratorPath;
            selected: lvPages.currentIndex === model.index;

            onClicked: {
                lvPages.currentIndex = model.index;
                menuPanel.activePageIndex = model.index;
                menuPanel.activePageName = model["Name"];
                menuPanel.activeIcon = model["Icon"];
                menuPanel.activePageId = model["PageId"];

                lvPages.forceActiveFocus();
            }
        }
    }
}
