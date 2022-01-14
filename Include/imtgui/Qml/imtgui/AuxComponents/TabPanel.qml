import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: tabPanelContainer;
    height: 40;
    width: 1000;
    color: Style.theme === "Dark" ? Style.baseColor: Style.backgroundColor;

    property int selectedIndex: -1;
    property int count: 0;
    property string firstElementImageSource;
    property var headersArray: [];
    property alias model: list.model;
    signal closeItem(int index);

    function setFirstElementImageSource(source){
        tabPanelContainer.firstElementImageSource = source;
    }


    function clearHeadersArray(){
        while(tabPanelContainer.headersArray.length > 0)
            tabPanelContainer.headersArray.pop();
        tabPanelContainer.count = 0
    }

    function addToHeadersArray(str){
        tabPanelContainer.headersArray.push(str);
        tabPanelContainer.count = tabPanelContainer.headersArray.length
        tabPanelContainer.selectedIndex = tabPanelContainer.headersArray.length - 1
    }

    ListView{
        id: list;
        anchors.fill: parent;
        clip: true;
        orientation: ListView.Horizontal;

        spacing: 0;
        model: 4;
        interactive: false;
        delegate: TabDelegate{
            height: list.height;
            width: 200;
            selected: model.index == tabPanelContainer.selectedIndex;
            firstElement: model.index == 0;
            firstElementText: tabPanelContainer.firstElementName;
            firstElementImageSource: tabPanelContainer.firstElementImageSource;
            text: model.Title;
            onClicked: {
                console.log("TabDelegate onClicked")
                tabPanelContainer.selectedIndex = model.index;
            }

            onCloseSignal: {
                tabPanelContainer.closeItem(model.index);
            }

        }
    }
}
