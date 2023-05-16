import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting a feature package");
    removeMessage: qsTr("Delete the selected package ?");

    onRemoved: {
        featuresProvider.updateModel();
    }

    onRenamed: {
        featuresProvider.updateModel();
    }
}
