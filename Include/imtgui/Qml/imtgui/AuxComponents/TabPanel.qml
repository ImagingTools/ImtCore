import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: tabPanel;
    height: 40;
    width: 1000;
    color : Style.baseColor;

    property int selectedIndex: 0;
    property string firstElementName: "Packages";
    property string firstElementImageSource: "../../Icons/Workflow.svg";

   // property string textColor: Style.textColor;
    property string fontName: "";

    function setFirstElementImageSource(source){
        firstElementImageSource = source;
    }

    ListView{
        id: list;
        anchors.fill: parent;
        clip: true;
//        boundsBehavior: Flickable.StopAtBounds;
        orientation: ListView.Horizontal;
//        enabled: tabPanel.visible;
        spacing: 0;
        model: 2;
        delegate: TabDelegate{
            height: list.height;
            width: 150;
            selected: model.index == tabPanel.selectedIndex;
            firstElement: model.index == 0;
            firstElementText: tabPanel.firstElementName;
            firstElementImageSource: tabPanel.firstElementImageSource;
            text: "<no name>";
//            textColor: tabPanel.textColor;
            fontName: tabPanel.fontName;
            onClicked: {
                tabPanel.selectedIndex = model.index;
            }

        }
    }
}
