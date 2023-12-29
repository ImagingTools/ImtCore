import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Rectangle {
    id: menuPanel;

    color: Style.backgroundColor;

    radius: 7;

    clip: true;

    width: visible ? contentWidth : 1;

    property string textColor: Style.textColor;
    property string fontName: "Helvetica";
    property string activePageId;
    property string activePageName;
    property string activeIcon;
    property string firstElementImageSources: "";

    property int activePageIndex: -1;
    property int pagesCount: lvPages.count;

    property TreeItemModel model: TreeItemModel {};

    property int spacing: 0;

    property int count: lvPages.count;
    property int contentWidth: 0;

    signal activePageChanged;

    function clearModels(){
        lvPages.model = 0;
        menuPanel.activePageIndex = -1;
    }

    property bool isQtStyle: false;

    Component.onCompleted: {
        Events.subscribeEvent("QtStyle", menuPanel.setQtStyle);
        Events.subscribeEvent("AcfStyle", menuPanel.setAcfStyle);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("QtStyle", menuPanel.setQtStyle);
        Events.unSubscribeEvent("AcfStyle", menuPanel.setAcfStyle);

    }

    function setQtStyle(){
        menuPanel.isQtStyle = true;
    }

    function setAcfStyle(){
        menuPanel.isQtStyle = false;

    }

    function sendStyle(){
        if(menuPanel.isQtStyle){
            Events.sendEvent("QtStyle");
        }
        else {
            Events.sendEvent("AcfStyle");
        }
    }

    onModelChanged: {
        let pageIndex = menuPanel.activePageIndex;

        lvPages.model = 0;
        lvPages.model = menuPanel.model;
    }

    ListView {
        id: lvPages;

        anchors.fill: parent;

        boundsBehavior: Flickable.StopAtBounds;

        Keys.onPressed: {
            console.log("menuPanel.activePageIndex");
            if (event.key == Qt.Key_Up){
                if (menuPanel.activePageIndex >= 1){
                    menuPanel.activePageIndex--;
                }
            }

            if (event.key == Qt.Key_Down){
                if (menuPanel.activePageIndex < lvPages.count - 1){
                    menuPanel.activePageIndex++;
                }
            }
        }

        delegate: MenuPanelButton {
            Component.onCompleted: {
                if (model.index === 0 && menuPanel.activePageIndex === -1){
                    clicked();
                }
            }

            text:  model["Name"];
            textColor: Style.textColor;
            fontName: menuPanel.fontName;
            imageSource: (highlighted || selected) ? "../../../" + Style.theme + "/Icons/" + model["Icon"] + "_On_Selected.svg":
                                                     "../../../" + Style.theme + "/Icons/" + model["Icon"] + "_On_Normal.svg";
            decoratorSource : Style.menuButtonDecoratorPath;
            selected: menuPanel.activePageIndex === model.index;

            onClicked: {
//                lvPages.currentIndex = model.index;
                console.log("MenuPanel onClicked", model.index);
                menuPanel.activePageIndex = model.index;
                menuPanel.activePageName = model["Name"];
                menuPanel.activeIcon = model["Icon"];
                menuPanel.activePageId = model["Id"];

                lvPages.forceActiveFocus();
                menuPanel.sendStyle();
            }
        }
    }

}
