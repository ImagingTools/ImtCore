import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: tabPanelContainer;
    height: 40;
    width: 1000;
    color: Style.theme === "Dark" ? Style.baseColor: Style.backgroundColor;

    property int selectedIndex: 0;
//    property string firstElementName: "Packages";
    property string firstElementImageSource: "../../Icons/Workflow.svg";
    property var headersArray: [];

   // property string textColor: Style.textColor;
//    property string fontName: "";

    function setFirstElementImageSource(source){
        tabPanelContainer.firstElementImageSource = source;
    }


    function clearHeadersArray(){
        while(tabPanelContainer.headersArray.length > 0)
            tabPanelContainer.headersArray.pop();
        list.model = 0
    }

    function addToHeadersArray(str){
        tabPanelContainer.headersArray.push(str);
        list.model = tabPanelContainer.headersArray.length
    }

    ListView{
        id: list;
        anchors.fill: parent;
        clip: true;
//        boundsBehavior: Flickable.StopAtBounds;
        orientation: ListView.Horizontal;
//        enabled: tabPanelContainer.visible;
        spacing: 0;
        model: 0;
        interactive: false;
        delegate: TabDelegate{
            height: list.height;
            width: 150;
            selected: model.index == tabPanelContainer.selectedIndex;
            firstElement: model.index == 0;
            firstElementText: tabPanelContainer.firstElementName;
            firstElementImageSource: tabPanelContainer.firstElementImageSource;
            text: tabPanelContainer.headersArray[model.index];
//            textColor: tabPanelContainer.textColor;
//            fontName: Style.fontFamily;
            onClicked: {
                tabPanelContainer.selectedIndex = model.index;
            }

        }
    }
}
