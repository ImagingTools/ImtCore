import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

CollectionView {
    id: productCollectionViewContainer;

    visibleMetaInfo: false;

    Component.onCompleted: {
        productCollectionViewContainer.commandsDelegatePath = "../../imtlicgui/ProductCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("ProductId");
    }
}
