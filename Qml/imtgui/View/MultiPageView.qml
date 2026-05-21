import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
    id: root;

    property ListModel pagesModel: ListModel {
        dynamicRoles: true;
    }

    property int currentIndex: -1

    property int mainMargin: Style.marginM;
    property int panelWidth: Style.sizeHintXXXS;

    signal pageLoaded(int index, var pageItem, string pageId);

    function clear(){
        pagesModel.clear();
    }

    function addPage(pageId, pageName, pageComp, icon){
        pagesModel.append({id: pageId, name: pageName, SourceComponent: pageComp, icon: icon})
    }

    function removePage(pageId){
        let index = getIndexById(pageId);
        if (index >= 0){
            pagesModel.remove(index);
        }
    }

    function getPageById(pageId){
        let index = getIndexById(pageId);
        if (index >= 0){
            return getPageByIndex(index);
        }

        return null;
    }

    function getPageByIndex(index){
        if (index < 0 || bodyRepeater.count <= index){
            return null;
        }

        let loaderItem = bodyRepeater.itemAt(index)
        return loaderItem.item;
    }

    function getIndexById(pageId){
        for (let i = 0; i < pagesModel.count; i++){
            let id = pagesModel.get(i).id;
            if (id === pageId){
                return i;
            }
        }

        return -1;
    }

    // --- Left sidebar ---
    Item {
        id: sidebarPanel;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.topMargin: root.mainMargin;
        anchors.leftMargin: root.mainMargin;

        width: root.panelWidth;

        Column {
            id: sidebarColumn;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.topMargin: Style.marginS;

            spacing: Style.spacingXS;

            Repeater {
                model: root.pagesModel;

                delegate: Rectangle {
                    id: navItem;

                    width: sidebarColumn.width;
                    height: Style.controlHeightM;
                    radius: Style.marginS;
                    color: root.currentIndex === model.index
                        ? Style.selectedColor
                        : navMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"

                    Row {
                        anchors.fill: parent;
                        anchors.leftMargin: Style.marginM;
                        anchors.rightMargin: Style.marginM;
                        spacing: Style.marginS;

                        Image {
                            visible: model.icon
                            anchors.verticalCenter: parent.verticalCenter;
                            width: Style.iconSizeS;
                            height: Style.iconSizeS;
                            source: model.icon ? 
                                        root.currentIndex === model.index ? "qrc:/" + Style.getIconPath(model.icon, Icon.State.On, Icon.Mode.Selected) : "qrc:/" + Style.getIconPath(model.icon, Icon.State.On, Icon.Mode.Normal) :
                                        ""
                            sourceSize.width: width
                            sourceSize.height: height
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter;
                            text: model.name;
                            font.family: Style.fontFamily;
                            font.pixelSize: Style.fontSizeM;
                            font.bold: root.currentIndex === model.index;
                            color: root.currentIndex === model.index ? Style.textSelectedColor : Style.textColor;
                            elide: Text.ElideRight;
                            width: parent.width - parent.anchors.leftMargin - parent.anchors.rightMargin - (model.icon ? Style.iconSizeS + parent.spacing : 0);
                        }
                    }

                    MouseArea {
                        id: navMouseArea;

                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: Qt.PointingHandCursor;

                        onClicked: {
                            root.currentIndex = model.index;
                        }
                    }

                    Component.onCompleted: {
                        if (model.index === 0){
                            root.currentIndex = 0
                        }
                    }
                }
            }
        }
    }

    // --- Vertical separator ---
    Rectangle {
        id: separator;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: sidebarPanel.right;
        anchors.topMargin: root.mainMargin;
        anchors.bottomMargin: root.mainMargin;
        anchors.leftMargin: root.mainMargin;

        width: 1;
        color: Style.borderColor;
    }

    // --- Content area ---
    Item {
        id: bodyAdministration;

        anchors.left: separator.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.topMargin: root.mainMargin;
        anchors.leftMargin: root.mainMargin;
        anchors.rightMargin: root.mainMargin;

        Repeater {
            id: bodyRepeater;

            anchors.fill: parent;

            model: root.pagesModel;

            delegate: Loader {
                id: bodyLoader;

                anchors.fill: parent;

                sourceComponent: model.SourceComponent;

                visible: root.currentIndex === model.index;

                onLoaded: {
                    root.pageLoaded(model.index, item, model.id);
                }

                onStatusChanged: {
                    if (status === Loader.Error){
                        console.error("Unable to load component: ", bodyLoader.source);
                    }
                }
            }
        }
    }
}
