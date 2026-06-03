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
        pagesModel.append({id: pageId, name: pageName, SourceComponent: pageComp, icon: icon, parentId: "", isSubpage: false, expanded: false})
    }

    function addSubPage(parentId, pageId, pageName, pageComp, icon){
        var parentIndex = getIndexById(parentId)
        if (parentIndex < 0) {
            return
        }
        // Mark parent as having subpages
        pagesModel.setProperty(parentIndex, "expanded", true)
        // Find insertion point (after parent and existing subpages of this parent)
        var insertAt = parentIndex + 1
        while (insertAt < pagesModel.count) {
            var item = pagesModel.get(insertAt)
            if (item.parentId !== parentId) {
                break
            }
            insertAt++
        }
        pagesModel.insert(insertAt, {id: pageId, name: pageName, SourceComponent: pageComp, icon: icon, parentId: parentId, isSubpage: true, expanded: false})
    }

    function removePage(pageId){
        // Remove subpages first
        for (var i = pagesModel.count - 1; i >= 0; i--) {
            if (pagesModel.get(i).parentId === pageId) {
                pagesModel.remove(i)
            }
        }
        var index = getIndexById(pageId);
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

    function hasSubPages(pageId) {
        for (var i = 0; i < pagesModel.count; i++) {
            if (pagesModel.get(i).parentId === pageId) {
                return true
            }
        }
        return false
    }

    function toggleExpanded(pageId) {
        var idx = getIndexById(pageId)
        if (idx >= 0) {
            var current = pagesModel.get(idx).expanded
            pagesModel.setProperty(idx, "expanded", !current)
        }
    }

    function isExpanded(pageId) {
        var idx = getIndexById(pageId)
        if (idx >= 0) {
            return pagesModel.get(idx).expanded
        }
        return false
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

                    readonly property bool __isParentWithSubs: root.hasSubPages(model.id)
                    readonly property bool __isSubpage: model.isSubpage
                    readonly property string __parentId: model.parentId || ""
                    readonly property bool __parentExpanded: __isSubpage ? root.isExpanded(__parentId) : true

                    visible: !__isSubpage || __parentExpanded
                    width: sidebarColumn.width;
                    height: visible ? Style.controlHeightM : 0;
                    radius: Style.marginS;
                    color: root.currentIndex === model.index
                        ? Style.selectedColor
                        : navMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"

                    Row {
                        anchors.fill: parent;
                        anchors.leftMargin: navItem.__isSubpage ? Style.marginM + Style.marginL : Style.marginM;
                        anchors.rightMargin: Style.marginM;
                        spacing: Style.marginS;

                        // Expand/collapse arrow for parents with subpages
                        Image {
                            visible: navItem.__isParentWithSubs
                            anchors.verticalCenter: parent.verticalCenter;
                            width: Style.iconSizeXS;
                            height: Style.iconSizeXS;
                            source: root.isExpanded(model.id)
                                ? "qrc:/" + Style.getIconPath("Icons/ArrowDown", Icon.State.On, Icon.Mode.Normal)
                                : "qrc:/" + Style.getIconPath("Icons/ArrowRight", Icon.State.On, Icon.Mode.Normal)
                            sourceSize.width: width
                            sourceSize.height: height
                        }

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
                            font.pixelSize: navItem.__isSubpage ? Style.fontSizeS : Style.fontSizeM;
                            font.bold: root.currentIndex === model.index;
                            color: root.currentIndex === model.index ? Style.textSelectedColor : Style.textColor;
                            elide: Text.ElideRight;
                            width: parent.width - parent.anchors.leftMargin - parent.anchors.rightMargin
                                - (model.icon ? Style.iconSizeS + parent.spacing : 0)
                                - (navItem.__isParentWithSubs ? Style.iconSizeXS + parent.spacing : 0);
                        }
                    }

                    MouseArea {
                        id: navMouseArea;

                        anchors.fill: parent;
                        hoverEnabled: true;
                        cursorShape: Qt.PointingHandCursor;

                        onClicked: {
                            if (navItem.__isParentWithSubs) {
                                root.toggleExpanded(model.id)
                            }
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
