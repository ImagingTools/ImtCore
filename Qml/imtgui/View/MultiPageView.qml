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

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    function clear(){
        pagesModel.clear();
    }

    function addPage(pageId, pageName, pageComp){
        pagesModel.append({id: pageId, name: pageName, SourceComponent: pageComp})
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

    Item {
        id: mainPanelBackground;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.topMargin: root.mainMargin;
        anchors.leftMargin: root.mainMargin;

        width: root.panelWidth;

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
            anchors.topMargin: root.mainMargin;

            spacing: Style.marginM;

            ListView {
                id: mainPanelRepeater;

                width: parent.width;
                height: contentHeight;

                model: root.pagesModel;

                spacing: Style.marginM;

                boundsBehavior: Flickable.StopAtBounds;

                delegate: ItemDelegate {
                    text: model.name;

                    highlighted: root.currentIndex === model.index;

                    onClicked: {
                        root.currentIndex = model.index;
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

    Item {
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
