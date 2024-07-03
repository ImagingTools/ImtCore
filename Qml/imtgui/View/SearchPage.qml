import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

ViewBase {
    id: root;

    model: TreeItemModel {}

    commandsControllerComp: Component {
        CommandsController {
            id: commandsController;

            isReady: true;

            Component.onCompleted: {
                commandsController.commandsModel = commandsModel2;
            }
        }
    }

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

                            MainDocumentManager.openDocument(typeId, elementId);
                        }
                    }
                }
            }
        }
    }

    TreeItemModel {
        id: commandsModel2;

        Component.onCompleted: {
            let index = commandsModel2.insertNewItem();
            commandsModel2.setData("Id", "Open", index);
            commandsModel2.setData("Name", "Open", index);
            commandsModel2.setData("Description", "", index);
            commandsModel2.setData("IsEnabled", false, index);
            commandsModel2.setData("Visible", true, index);
            commandsModel2.setData("Icon", "Icons/Edit", index);
            commandsModel2.setData("Status", "", index);

            if (root.commandsController){
                root.commandsController.commandsModel = commandsModel2;
            }
        }
    }

    Component.onCompleted: {
        Events.subscribeEvent("GlobalSearchActivated", root.update)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("GlobalSearchActivated", root.update)
    }

    function clearAll(){
        console.log("clearAll");

        root.model.clear();
        table.elements = 0;
        tabPanel.selectedIndex = 0;
    }

    function update(text){
        console.log("update", text);
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

    TreeItemModel {
        id: headers;

        Component.onCompleted: {
            let index = headers.insertNewItem();
            headers.setData("Id", "Description", index)
            headers.setData("Name", "Description", index)

            table.headers = headers;
        }
    }

    TabPanel {
        id: tabPanel;
        anchors.top: parent.top;
        width: parent.width;
        model: root.model;
        displayRoleId: "Name";
        isCloseEnable: false;
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
                    color: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected ? Style.alternateBaseColor: "transparent": "transparent";
                }

                Rectangle {
                    anchors.right: tabPanelDecorator.right;
                    anchors.verticalCenter: tabPanelDecorator.verticalCenter;
                    width: 1;
                    height: tabPanelDecorator.height / 2;
                    visible: tabPanelDecorator.baseElement
                             ? tabPanelDecorator.baseElement.index < tabPanelDecorator.baseElement.listView.count - 1 &&
                               !tabPanelDecorator.baseElement.selected &&
                               (tabPanelDecorator.baseElement.index + 1) !== tabPanelDecorator.baseElement.selectedIndex
                             : false;

                    color: Style.borderColor2;
                }

                Rectangle {
                    id: selection;
                    anchors.bottom: tabPanelDecorator.bottom;
                    anchors.left: tabPanelDecorator.left;
                    anchors.right: tabPanelDecorator.right;
                    height: 2;
                    color: Style.tabSelectedColor;
                    visible: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected: false;
                }

                Row {
                    id: content;
                    anchors.centerIn: tabPanelDecorator;
                    height: tabPanelDecorator.height;
                    spacing: Style.size_mainMargin;

                    Text {
                        id: text;
                        anchors.verticalCenter: parent.verticalCenter;
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

    Table {
        id: table;
        anchors.top: tabPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        onSelectionChanged: {
            console.log("onSelectionChanged", selection);
            if (root.commandsController){
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
            console.log("gqlModel send", text);

            var query = Gql.GqlRequest("query", "Search");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Text", text);
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (this.state === "Ready"){
                console.log("Search onStateChanged", this.toJson());

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


