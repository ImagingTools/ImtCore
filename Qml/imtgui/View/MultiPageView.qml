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
        root.currentIndex = -1;
    }

    function addPage(pageId, pageName, pageComp, icon){
        pagesModel.append({
            id: pageId,
            name: pageName,
            SourceComponent: pageComp,
            icon: icon,
            parentId: "",
            isSubpage: false,
            expanded: false,
            loaded: false,
            submenuVisited: false,
            lastSubpageId: "",
            badge: ""
        })
    }

    function addSubPage(parentId, pageId, pageName, pageComp){
        var parentIndex = getIndexById(parentId)
        if (parentIndex < 0) {
            return
        }

        var insertAt = parentIndex + 1
        while (insertAt < pagesModel.count) {
            var item = pagesModel.get(insertAt)
            if (item.parentId !== parentId) {
                break
            }
            insertAt++
        }

        pagesModel.insert(insertAt, {
            id: pageId,
            name: pageName,
            SourceComponent: pageComp,
            icon: "",
            parentId: parentId,
            isSubpage: true,
            expanded: false,
            loaded: false,
            submenuVisited: false,
            lastSubpageId: "",
            badge: ""
        })
    }

    // Small text badge next to a page's nav label (e.g. "3", or "•" as an "unread" dot) -
    // badgeText: "" clears it. No built-in caller; consumers (e.g. AgentEditor's Services/Log
    // pages) call this whenever the count/marker they want shown changes.
    function setPageBadge(pageId, badgeText){
        var idx = getIndexById(pageId)
        if (idx >= 0) {
            pagesModel.setProperty(idx, "badge", badgeText)
        }
    }

    function removePage(pageId){
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
        if (!loaderItem) {
            return null
        }

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

    function getFirstSubPageIndex(parentId) {
        for (var i = 0; i < pagesModel.count; i++) {
            if (pagesModel.get(i).parentId === parentId) {
                return i
            }
        }
        return -1
    }

    function getLastSubPageIndex(parentId) {
        var parentIndex = getIndexById(parentId)
        if (parentIndex < 0) {
            return -1
        }

        var lastSubpageId = pagesModel.get(parentIndex).lastSubpageId
        if (lastSubpageId) {
            var lastIndex = getIndexById(lastSubpageId)
            if (lastIndex >= 0 && pagesModel.get(lastIndex).parentId === parentId) {
                return lastIndex
            }
        }

        return getFirstSubPageIndex(parentId)
    }

    function rememberSubPageSelection(index) {
        if (index < 0 || index >= pagesModel.count) {
            return
        }

        var selectedPage = pagesModel.get(index)
        if (!selectedPage.isSubpage) {
            return
        }

        var parentIndex = getIndexById(selectedPage.parentId)
        if (parentIndex < 0) {
            return
        }

        pagesModel.setProperty(parentIndex, "submenuVisited", true)
        pagesModel.setProperty(parentIndex, "lastSubpageId", selectedPage.id)
    }

    function ensurePageLoaded(index) {
        if (index < 0 || index >= pagesModel.count) {
            return
        }

        if (!pagesModel.get(index).loaded) {
            pagesModel.setProperty(index, "loaded", true)
        }
    }

    function activatePage(index) {
        if (index < 0 || index >= pagesModel.count) {
            return
        }

        ensurePageLoaded(index)
        root.currentIndex = index
    }

    onCurrentIndexChanged: {
        if (root.currentIndex < 0 || root.currentIndex >= pagesModel.count) {
            return
        }

        var currentPage = pagesModel.get(root.currentIndex)
        if (!currentPage.isSubpage && root.hasSubPages(currentPage.id) && root.isExpanded(currentPage.id)) {
            var subTarget = currentPage.submenuVisited
                ? root.getLastSubPageIndex(currentPage.id)
                : root.getFirstSubPageIndex(currentPage.id)
            if (subTarget >= 0 && subTarget !== root.currentIndex) {
                root.activatePage(subTarget)
                return
            }
        }

        ensurePageLoaded(root.currentIndex)
        if (currentPage.isSubpage) {
            rememberSubPageSelection(root.currentIndex)
        }
    }

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

                delegate: Item {
                    id: navDelegate

                    // Test instrumentation: every MultiPageView consumer (e.g. Administration's
                    // Roles/Users/Groups) gets a per-nav-item selector keyed by the same pageId passed
                    // to addPage()/addSubPage() - previously this delegate had no objectName at all, so
                    // subpage navigation was only clickable by visible text. Inert - no runtime/visual
                    // effect.
                    objectName: "Page_" + model.id;

                    readonly property bool __isParentWithSubs: root.hasSubPages(model.id)
                    readonly property bool __isSubpage: model.isSubpage
                    readonly property string __parentId: model.parentId || ""
                    readonly property bool __parentExpanded: __isSubpage ? root.isExpanded(__parentId) : true
                    readonly property bool __isSelected: root.currentIndex === model.index

                    visible: !__isSubpage || __parentExpanded
                    width: sidebarColumn.width
                    height: visible ? Style.controlHeightM : 0

                    Rectangle {
                        id: navItem
                        anchors.fill: parent
                        radius: Style.marginS
                        color: navDelegate.__isSelected
                            ? Style.selectedColor
                            : navMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: navDelegate.__isSubpage
                                ? Style.marginM + Style.iconSizeS + Style.marginS
                                : Style.marginM
                            anchors.rightMargin: Style.marginM
                            spacing: Style.marginS

                            Image {
                                visible: !navDelegate.__isSubpage && model.icon
                                anchors.verticalCenter: parent.verticalCenter
                                width: Style.iconSizeS
                                height: Style.iconSizeS
                                source: model.icon
                                    ? (navDelegate.__isSelected
                                        ? "qrc:/" + Style.getIconPath(model.icon, Icon.State.On, Icon.Mode.Selected)
                                        : "qrc:/" + Style.getIconPath(model.icon, Icon.State.On, Icon.Mode.Normal))
                                    : ""
                                sourceSize.width: width
                                sourceSize.height: height
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: model.name
                                font.family: Style.fontFamily
                                font.pixelSize: navDelegate.__isSubpage ? Style.fontSizeS : Style.fontSizeM
                                font.bold: navDelegate.__isSelected && !navDelegate.__isSubpage
                                color: navDelegate.__isSelected ? Style.textSelectedColor : Style.textColor
                                elide: Text.ElideRight
                                width: parent.width - ((!navDelegate.__isSubpage && model.icon) ? Style.iconSizeS + parent.spacing : 0)
                                    - (navBadge.visible ? navBadge.width + parent.spacing : 0)
                            }

                            // Small text badge next to the label (a count like "3", or a short
                            // marker like "•") - see setPageBadge(). Empty/unset by default, so
                            // pages that never call it look exactly as before.
                            Text {
                                id: navBadge
                                visible: model.badge !== undefined && model.badge !== ""
                                anchors.verticalCenter: parent.verticalCenter
                                text: model.badge !== undefined ? model.badge : ""
                                font.family: Style.fontFamily
                                font.pixelSize: Style.fontSizeS
                                color: navDelegate.__isSelected ? Style.textSelectedColor : Style.subtitleColor
                            }
                        }

                        Image {
                            visible: navDelegate.__isParentWithSubs
                            anchors.right: navItem.right
                            anchors.rightMargin: Style.marginS
                            anchors.verticalCenter: navItem.verticalCenter
                            width: Style.iconSizeXS
                            height: Style.iconSizeXS
                            source: root.isExpanded(model.id)
                                ? "qrc:/" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
                                : "qrc:/" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
                            sourceSize.width: width
                            sourceSize.height: height
                        }

                        MouseArea {
                            id: navMouseArea

                            // Test instrumentation: matches the "[objectName=Page_x] [objectName=MouseArea]"
                            // click convention used everywhere else in this codebase. Inert.
                            objectName: "MouseArea";

                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (navDelegate.__isParentWithSubs) {
                                    var wasExpanded = root.isExpanded(model.id)
                                    root.toggleExpanded(model.id)

                                    if (!wasExpanded) {
                                        var targetSubIndex = root.pagesModel.get(model.index).submenuVisited
                                            ? root.getLastSubPageIndex(model.id)
                                            : root.getFirstSubPageIndex(model.id)

                                        if (targetSubIndex >= 0) {
                                            root.activatePage(targetSubIndex)
                                            return
                                        }
                                    } else {
                                        root.currentIndex = model.index
                                        return
                                    }
                                }

                                root.activatePage(model.index)
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: separator;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: sidebarPanel.right;
        anchors.leftMargin: root.mainMargin;

        width: 1;
        color: Style.borderColor;
        opacity: 0.5
    }

    Item {
        anchors.left: separator.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;

        Repeater {
            id: bodyRepeater;

            anchors.fill: parent;

            model: root.pagesModel;

            delegate: Loader {
                id: bodyLoader;

                anchors.fill: parent;

                active: model.loaded === true;
                sourceComponent: model.SourceComponent;

                visible: active && root.currentIndex === model.index;
                opacity: visible ? 1.0 : 0.0;

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
