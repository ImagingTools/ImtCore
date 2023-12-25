import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolqml 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting a feature");
    removeMessage: qsTr("Delete the selected feature ?");

    onRemoved: {
        FeaturesProvider.updateModel();
    }

    onRenamed: {
        FeaturesProvider.updateModel();
    }
}
