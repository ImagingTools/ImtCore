import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: root;

    property ListModel tabModel: ListModel {
        dynamicRoles: true;
    }

    property alias currentIndex: tabPanel.selectedIndex;

    property int mainMargin: Style.size_mainMargin;

    signal tabLoaded(int index, var tabItem);


    function clear(){
        tabModel.clear();
    }

    function addTab(tabId, tabName, tabComp){
        tabModel.append({Id: tabId, Title: tabName, SourceComponent: tabComp})
    }

    function removeTab(tabId){
        let index = getIndexById(tabId);
        if (index >= 0){
            tabModel.remove(index);
        }
    }

    function getTabById(tabId){
        let index = getIndexById(tabId);
        if (index >= 0){
            return getTabByIndex(index);
        }

        return null;
    }

    function getTabByIndex(index){
        if (index < 0 || bodyRepeater.count <= index){
            return null;
        }

        let loaderItem = bodyRepeater.itemAt(index)
        return loaderItem.item;
    }

    function getIndexById(tabId){
        for (let i = 0; i < tabModel.count; i++){
            let id = tabModel.get(i).Id;
            if (id === tabId){
                return i;
            }
        }

        return -1;
    }

    TabPanel {
        id: tabPanel;
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        model: root.tabModel;
        isCloseEnable: false
        clip: true;
        onRightClicked: {
            if (tabPanel.selectedIndex < root.tabModel.count - 1){
                tabPanel.selectedIndex++;
            }
        }

        onLeftClicked: {
            if (tabPanel.selectedIndex > 0){
                tabPanel.selectedIndex--;
            }
        }
    }

    Rectangle {
        id: bodyTab;
        z: 5;
        anchors.left: parent.left;
        anchors.top: tabPanel.bottom;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        color: Style.backgroundColor2;

        Repeater {
            id: bodyRepeater;
            anchors.fill: parent;
            model: root.tabModel;
            delegate: Loader {
                id: bodyLoader;
                anchors.fill: parent;
                sourceComponent: model.SourceComponent;
                visible: tabPanel.selectedIndex == model.index;

                onLoaded: {
                    root.tabLoaded(model.index, item);
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
