import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

FilePickerElementView {
    id: root;

    path: model.Path;
    name: model.Name;

    Component.onCompleted: {
         fileDialog.currentFolder = model.Path;
    }

    onAccepted: {
        model.Path = root.path;
    }
}

