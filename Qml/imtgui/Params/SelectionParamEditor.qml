import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

ComboBoxElementView {
    id: root;
    name: internal.modelData.Name;
    model: internal.modelData.Parameters;
    currentIndex: internal.modelData.Value;
    controlWidth: 300;

    onCurrentIndexChanged: {
        let modelValue = internal.modelData.Value;
        if (modelValue !== currentIndex){
            internal.modelData.Value = currentIndex;
        }
    }

    QtObject {
        id: internal;
        property var modelData: model;
    }
}


