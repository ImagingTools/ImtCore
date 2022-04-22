import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
//import '../AuxComponents'
//import imtgui.AuxComponents 1.0

CollectionView {
    id: productCollectionView;

    anchors.fill: parent;

    width: 100;
    height: 50;

    color: Style.backgroundColor;

    property Item multiDocViewItem;
    property Item rootItem;

    Component.onCompleted: {
    }

}
