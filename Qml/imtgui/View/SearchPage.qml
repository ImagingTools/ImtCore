import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

ViewBase {
    id: root;

    model: TreeItemModel {}

    commandsDelegateComp: Component {
        ViewCommandsDelegateBase {
            onCommandActivated: {
                if (commandId == "Open"){
                    let indexes = table.getSelectedIndexes();
                    if (indexes.length > 0){
                        if (tabPanel.selectedIndex >= 0){
                            let typeId = root.model.getData("Id", tabPanel.selectedIndex);
                            let elements = table.elements;
                            let elementId = elements.getData("Id", indexes[0])
                            let elementTypeId = elements.getData("TypeId", indexes[0])

                            MainDocumentManager.openDocument(typeId, elementId, elementTypeId);
                        }
                    }
                }
            }
        }
    }

    onModelChanged: {
        tabPanel.visible = model.getItemsCount() > 0;
    }

    Component.onCompleted: {
        checkWidth();
        Events.subscribeEvent("GlobalSearchActivated", root.update)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("GlobalSearchActivated", root.update)
    }

    function clearAll(){
        root.model.clear();
        table.elements = 0;
        tabPanel.selectedIndex = 0;
        tabPanel.visible = false;
    }

    function update(text){
        clearAll();

        if (text !== ""){
            gqlModel.send(text);
        }
    }

    function updateGui(){
        if (tabPanel.selectedIndex >= 0){
            if (root.model.containsKey("Items", tabPanel.selectedIndex)){
                table.elements = root.model.getData("Items", tabPanel.selectedIndex);
            }
        }
    }

    onWidthChanged: {
        checkWidth();
    }

    property int contentWidth: 700;

    function checkWidth(){
        if (width - 2 * Style.size_mainMargin <= root.contentWidth){
            table.width = width - 2 * Style.size_mainMargin;
        }
        else{
            table.width = root.contentWidth;
        }
    }

    Rectangle {
        id: noResultsRect;
        anchors.fill: parent;
        color: Style.backgroundColor2;
        visible: !tabPanel.visible;

        BaseText {
            anchors.centerIn: parent;
            text: qsTr("No results");
            font.family: Style.fontFamilyBold;
            font.bold: true;
        }
    }

    TabPanel {
        id: tabPanel;
        anchors.top: parent.top;
        anchors.topMargin: Style.size_mainMargin;
        anchors.horizontalCenter: parent.horizontalCenter;
        height: 30;
        model: root.model;
        displayRoleId: "Name";
        visible: false;
        spacing: Style.size_mainMargin;
        color: Style.backgroundColor2;
        onSelectedIndexChanged: {
            table.resetSelection();
            root.doUpdateGui();
        }
        tabDelegateDecorator: Component {
            DecoratorBase {
                id: tabPanelDecorator;

                width: content.width + 2 * Style.size_mainMargin;
                height: baseElement ? baseElement.height : 50

                Rectangle {
                    anchors.fill: tabPanelDecorator;
                    color: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected ? Style.selectedColor: "transparent";
                    radius: 15;
                    border.width: 1;
                    border.color: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected ? Style.iconColorOnSelected: Style.borderColor;
                }

                Row {
                    id: content;
                    anchors.centerIn: tabPanelDecorator;
                    height: tabPanelDecorator.height;
                    spacing: Style.size_mainMargin;

                    Text {
                        id: text;
                        anchors.verticalCenter: content.verticalCenter;
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";
                        elide: Text.ElideRight;
                    }
                }
            }
        }
    }

    Rectangle {
        id: separator;
        anchors.top: tabPanel.bottom;
        anchors.topMargin: Style.size_mainMargin;
        width: parent.width;
        height: 1;
        color: Style.borderColor;
    }

    Table {
        id: table;
        anchors.top: separator.bottom;
        anchors.topMargin: Style.size_mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_mainMargin;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: root.contentWidth;
        visible: tabPanel.visible;
        showHeaders: false;
        backgroundElementsColor: Style.backgroundColor2;
        elementsSpacing: Style.size_mainMargin;
        enableAlternating: false;
        itemHeight: 70;
        rowDelegate: Component {
            TableRowDelegateBase {
                id: tableDelegate;
                tableItem: table
                width: table.elementsListWidth - Style.size_mainMargin;
                height: elementView.height;
                selectedOpacity: 0;
                hoverOpacity: 0;

                ElementView {
                    id: elementView;
                    z: tableDelegate.z + 1;
                    width: tableDelegate.width;
                    name: model.Name;
                    description: model.Description;
                    color: tableDelegate.mouseArea.containsMouse || tableDelegate.selected ? Style.selectedColor : Style.baseColor;
                    controlComp: tableDelegate.mouseArea.containsMouse ? button : undefined;

                    Component {
                        id: button;
                        ToolButton {
                            width: 20;
                            height: width;
                            iconSource: "../../../" + Style.getIconPath("Icons/ExternalLink", Icon.State.On, Icon.Mode.Normal);
                            tooltipText: qsTr("Go to the content");
                            onClicked: {
                                table.select(model.index);
                                table.doubleClicked("", model.index)
                            }
                        }
                    }
                }
            }
        }

        onSelectionChanged: {
            if (root.commandsController){
                let selection = table.getSelectedIndexes();
                root.commandsController.setCommandIsEnabled("Open", selection.length > 0);
            }
        }

        onDoubleClicked: {
            if (root.commandsDelegate){
                root.commandsDelegate.commandHandle("Open");
            }
        }
    }

    Loading {
        id: loading;
        anchors.fill: parent;
        visible: gqlModel.state === "Loading";
    }

    GqlModel {
        id: gqlModel;

        function send(text){
            var query = Gql.GqlRequest("query", "Search");
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Text", text);
            query.AddParam(inputParams);
            var gqlData = query.GetQuery();
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                let resultModel;
                if (this.containsKey("data")){
                    resultModel = this.getData("data")
                }

                if (resultModel && resultModel.containsKey("Search")){
                    resultModel = resultModel.getData("Search")
                }

                if (resultModel){
                    root.model = resultModel;
                }

                root.doUpdateGui();
            }
        }
    }//GqlModel itemModel
}


