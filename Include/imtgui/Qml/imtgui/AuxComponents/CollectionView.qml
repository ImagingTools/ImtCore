import QtQuick 2.12
import Acf 1.0
//import '../UxAdvancedComponents' as AUX


Rectangle {
    id: collectionView;
    height: 100;
    width: 100;
    color: "transparent";
//    property alias tabPanel: tabPanelInternal;
    property TabPanel tabPanel: tabPanelInternal;
    property AuxTable table: tableInternal;
//    color : Style.baseColor;

    TabPanel {
        id:tabPanelInternal;
        anchors.left: parent.left;
        anchors.right: parent.right;
        visible: true;
    }

    AuxTable {
        id: tableInternal;
        width: parent.width;
        anchors.top: tabPanelInternal.bottom;
        anchors.bottom: parent.bottom;
    }

}
