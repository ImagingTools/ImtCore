import QtQuick 2.12
import imtauthTenantsSdl 1.0

Item {
    id: root
    width: 1200
    height: 720

    // Данные по SDL (existing tenant).
    TenantData {
        id: tenantData
        m_id: "tenant-001"
        m_name: "ACME Tenant"
        m_creatorId: "user-001"
        m_ownerId: "user-001"
        m_currentUserId: "user-001"
    }

    readonly property bool isNewTenant: !tenantData.m_id || tenantData.m_id === ""
    readonly property bool isCreator: tenantData.m_currentUserId && tenantData.m_currentUserId === tenantData.m_creatorId
    readonly property bool isOwner: tenantData.m_currentUserId && tenantData.m_currentUserId === tenantData.m_ownerId
    readonly property bool canManageMembers: isCreator || isOwner

    property var pagesModel: []
    property int currentIndex: -1

    function clearPages() {
        pagesModel = []
        currentIndex = -1
    }

    function addPage(pageId, pageName, pageComp) {
        var arr = pagesModel.slice()
        arr.push({
            id: pageId,
            name: pageName,
            sourceComponent: pageComp,
            parentId: "",
            isSubpage: false,
            expanded: false,
            submenuVisited: false,
            lastSubpageId: ""
        })
        pagesModel = arr
    }

    function addSubPage(parentId, pageId, pageName, pageComp) {
        var parentIndex = getIndexById(parentId)
        if (parentIndex < 0)
            return

        var insertAt = parentIndex + 1
        while (insertAt < pagesModel.length && pagesModel[insertAt].parentId === parentId)
            insertAt++

        var arr = pagesModel.slice()
        arr.splice(insertAt, 0, {
            id: pageId,
            name: pageName,
            sourceComponent: pageComp,
            parentId: parentId,
            isSubpage: true,
            expanded: false,
            submenuVisited: false,
            lastSubpageId: ""
        })
        pagesModel = arr
    }

    function getIndexById(pageId) {
        for (var i = 0; i < pagesModel.length; i++) {
            if (pagesModel[i].id === pageId)
                return i
        }
        return -1
    }

    function hasSubPages(pageId) {
        for (var i = 0; i < pagesModel.length; i++) {
            if (pagesModel[i].parentId === pageId)
                return true
        }
        return false
    }

    function isExpanded(pageId) {
        var idx = getIndexById(pageId)
        return idx >= 0 ? !!pagesModel[idx].expanded : false
    }

    function toggleExpanded(pageId) {
        var idx = getIndexById(pageId)
        if (idx < 0)
            return

        var arr = pagesModel.slice()
        arr[idx] = Object.assign({}, arr[idx], { expanded: !arr[idx].expanded })
        pagesModel = arr
    }

    function getFirstSubPageIndex(parentId) {
        for (var i = 0; i < pagesModel.length; i++) {
            if (pagesModel[i].parentId === parentId)
                return i
        }
        return -1
    }

    function getCurrentPage() {
        if (currentIndex < 0 || currentIndex >= pagesModel.length)
            return null
        return pagesModel[currentIndex]
    }

    function getPageByIndex(index) {
        if (index < 0 || index >= pagesModel.length)
            return null
        return pagesModel[index]
    }

    function isPageVisible(index) {
        var page = getPageByIndex(index)
        if (!page)
            return false
        if (!page.isSubpage)
            return true
        return isExpanded(page.parentId)
    }

    function activatePage(index) {
        if (index < 0 || index >= pagesModel.length)
            return
        currentIndex = index

        var page = pagesModel[index]
        if (page.isSubpage) {
            var parentIndex = getIndexById(page.parentId)
            if (parentIndex >= 0) {
                var arr = pagesModel.slice()
                arr[parentIndex] = Object.assign({}, arr[parentIndex], {
                    submenuVisited: true,
                    lastSubpageId: page.id
                })
                pagesModel = arr
            }
        }
    }

    function clickMenuById(pageId) {
        var index = getIndexById(pageId)
        if (index < 0)
            return

        if (hasSubPages(pageId)) {
            var wasExpanded = isExpanded(pageId)
            toggleExpanded(pageId)

            if (!wasExpanded) {
                var subIndex = getFirstSubPageIndex(pageId)
                if (subIndex >= 0) {
                    activatePage(subIndex)
                    return
                }
            } else {
                // Ровно как в MultiPageView: при втором клике выбирается parent.
                // У Relationships sourceComponent === null, значит контент пустой.
                currentIndex = index
                return
            }
        }

        activatePage(index)
    }

    function updatePagesLikeTenantEditor() {
        clearPages()
        addPage("General", "General", {})

        if (!isNewTenant) {
            addPage("Members", "Members", {})
            if (canManageMembers) {
                addPage("Roles", "Roles", {})
                addPage("Groups", "Groups", {})
            }
            if (isCreator) {
                addPage("Permissions", "Permissions", {})
                addPage("CrossOrgGrants", "Grants", {})
                addPage("Relationships", "Relationships", null)
                addSubPage("Relationships", "ConnectionCode", "Connection Code", {})
                addSubPage("Relationships", "ConnectOrganization", "Connect Organization", {})
                addSubPage("Relationships", "MyRelationships", "My Relationships", {})
            }
        }

        currentIndex = 0
    }

    Rectangle {
        anchors.fill: parent
        color: "#f8fafc"
    }

    Row {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Rectangle {
            width: 300
            height: parent.height
            radius: 8
            color: "#ffffff"
            border.color: "#e2e8f0"

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                Text {
                    text: "Tenant menu"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#0f172a"
                }

                Repeater {
                    model: root.pagesModel

                    delegate: Rectangle {
                        required property int index
                        required property var modelData

                        visible: root.isPageVisible(index)
                        width: parent.width
                        height: visible ? 36 : 0
                        radius: 6
                        color: root.currentIndex === index ? "#dbeafe" : "transparent"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: modelData.isSubpage ? 24 : 8
                            text: modelData.name + (root.hasSubPages(modelData.id) ? (root.isExpanded(modelData.id) ? "  ▼" : "  ▶") : "")
                            color: "#1e293b"
                            font.pixelSize: modelData.isSubpage ? 13 : 14
                            font.bold: root.currentIndex === index
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.clickMenuById(modelData.id)
                        }
                    }
                }
            }
        }

        Rectangle {
            width: parent.width - 316
            height: parent.height
            radius: 8
            color: "#ffffff"
            border.color: "#e2e8f0"

            Column {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 10

                Text {
                    text: "Current page"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#0f172a"
                }

                Text {
                    text: {
                        var p = root.getCurrentPage()
                        return p ? (p.name + " (id=" + p.id + ")") : "<none>"
                    }
                    color: "#334155"
                }

                Text {
                    text: {
                        var p = root.getCurrentPage()
                        if (!p)
                            return ""
                        return p.sourceComponent === null ? "No information" : "Information is shown"
                    }
                    color: {
                        var p = root.getCurrentPage()
                        return p && p.sourceComponent === null ? "#b91c1c" : "#166534"
                    }
                    font.pixelSize: 18
                    font.bold: true
                }
            }
        }
    }

    property int autoStep: 0

    Timer {
        id: autoRunTimer
        interval: 250
        repeat: true
        running: false

        onTriggered: {
            if (root.autoStep === 0) {
                var relationshipsIndex = root.getIndexById("Relationships")
                console.log("prolife32-init",
                            "existingTenant=" + (!root.isNewTenant),
                            "relationshipsExists=" + (relationshipsIndex >= 0),
                            "pagesCount=" + root.pagesModel.length)

                root.clickMenuById("Relationships")
                var pageAfterExpand = root.getCurrentPage()
                console.log("prolife32-expand",
                            "index=" + root.currentIndex,
                            "expanded=" + root.isExpanded("Relationships"),
                            "hasInfo=" + !!(pageAfterExpand && pageAfterExpand.sourceComponent !== null))
                root.autoStep = 1
                return
            }

            if (root.autoStep === 1) {
                root.clickMenuById("Relationships")
                var pageAfterCollapse = root.getCurrentPage()
                console.log("prolife32-collapse",
                            "index=" + root.currentIndex,
                            "expanded=" + root.isExpanded("Relationships"),
                            "noInfo=" + !!(pageAfterCollapse && pageAfterCollapse.sourceComponent === null))
                root.autoStep = 2
                return
            }

            autoRunTimer.stop()
            Qt.quit()
        }
    }

    Component.onCompleted: {
        updatePagesLikeTenantEditor()
        autoRunTimer.start()
    }
}
