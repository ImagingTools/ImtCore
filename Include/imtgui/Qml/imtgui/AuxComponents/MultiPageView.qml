import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    property ListModel pagesModel: ListModel {
        dynamicRoles: true;
    }

    property int selectedIndex: -1;
    property int mainMargin: 0;
    property int panelWidth: 150;

    signal pageLoaded(int index, var pageItem);

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    function addPage(pageId, pageName, pageSource, pageComp){
        pagesModel.append({Id: pageId, Name: pageName, Source: pageSource, SourceComponent: pageComp})
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
        console.log("getPageByIndex", index);
        console.log("bodyRepeater.count", bodyRepeater.count);
        if (index < 0 || bodyRepeater.count <= index){
            return null;
        }

        let loaderItem = bodyRepeater.itemAt(index)

        console.log("loaderItem", loaderItem.item);
        return loaderItem.item;
    }

    function getIndexById(pageId){
        for (let i = 0; i < pagesModel.count; i++){
            let id = pagesModel.get(i).Id;
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
        anchors.topMargin: root.mainMargin;
        anchors.leftMargin: root.mainMargin;

        width: root.panelWidth;

        color: Style.backgroundColor;

        Loader{
            id: mainPanelBackgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(mainPanelBackgroundDecoratorLoader.item){
                    mainPanelBackgroundDecoratorLoader.item.rootItem = mainPanelBackground;
                }
            }
        }

        Item{
            id: columnContainer;
            width: parent.width;
            height: mainPanel.height + 2*mainPanel.anchors.topMargin;

            Loader{
                id: mainPanelFrameLoader;

                anchors.fill: parent;

                sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;

                onLoaded: {
                    if(mainPanelFrameLoader.item){
                    }
                }
            }
        }

        Column {
            id: mainPanel;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.topMargin: 10;

            spacing: 10;

            Component{
                id: defaultButtonDecorator;

                CommonButtonDecorator{
                    height: 35;
                    color: Style.alternateBaseColor;
                }
            }

            Repeater {
                id: mainPanelRepeater;

                model: root.pagesModel;

                delegate:
                    BaseButton{
                    id: buttonContainer;

                    anchors.left: parent.left;
                    anchors.leftMargin: 10;
                    anchors.right: parent.right;
                    anchors.rightMargin: 10;

                    text:  model.Name;
                    selected: root.selectedIndex === model.index;

                    decorator: Style.commonButtonDecorator !==undefined ? Style.commonButtonDecorator : defaultButtonDecorator;

//                    Component.onCompleted: {
//                        if (model.index === 0){
//                            clicked();
//                        }
//                    }

                    onClicked: {
                        if (root.selectedIndex !== model.index){
                            root.selectedIndex = model.index;
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: bodyAdministration;

        anchors.left: mainPanelBackground.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.topMargin: root.mainMargin;
        anchors.leftMargin: root.mainMargin;
        anchors.rightMargin: root.mainMargin;

        z: 5;

        Repeater {
            id: bodyRepeater;

            model: root.pagesModel;

            delegate: Loader {
                id: bodyLoader;

                anchors.fill: parent;

                source: model.Source;
                sourceComponent: model.SourceComponent;

                visible: root.selectedIndex == model.index;

                onLoaded: {
                    root.pageLoaded(model.index, item);
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
