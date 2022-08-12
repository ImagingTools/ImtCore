import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

CollectionView {
    id: packageCollectionViewContainer;

    Component.onCompleted: {
        console.log("PackageCollection onCompleted", model.index);
        packageCollectionViewContainer.commandUpdateGui = "PackageCollectionUpdateGui";
        packageCollectionViewContainer.commandsDelegatePath = "../../imtlicgui/PackageCollectionViewCommandsDelegate.qml";
    }
}
