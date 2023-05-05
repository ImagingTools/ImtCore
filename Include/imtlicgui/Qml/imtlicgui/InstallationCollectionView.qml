import QtQuick 2.12
import imtgui 1.0

CollectionView {
    id: container;

    Component.onCompleted: {
        container.commandUpdateGui = "InstallationCollectionUpdateGui";
        container.commandsDelegatePath = "../../imtlicgui/InstallationCollectionViewCommandsDelegate.qml";
    }
}
