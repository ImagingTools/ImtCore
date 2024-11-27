import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

ComboBoxElementView {
    id: root;
    name: modelData ? modelData.Name : "";
    model: modelData ? modelData.Parameters : null;
    currentIndex: modelData ? modelData.Value : -1;
    controlWidth: 300;

    onCurrentIndexChanged: {
        let modelValue = modelData.Value;
        if (modelValue !== currentIndex){
            modelData.Value = currentIndex;
        }
    }

    property var modelData: 0;
}


