import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

 CollectionView {
     id: featureCollectionViewContainer;

     visibleMetaInfo: false;

     Component.onCompleted: {
         console.log("featureCollection onCompleted");
//         packageCollectionViewContainer.commandsDelegatePath = "../../imtlicgui/PackageCollectionViewCommandsDelegate.qml";
     }
 }
