import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBoxElementView {
    id: root;

    controlComp: cbComp;

    Component {
        id: cbComp;
        FilterableComboBox {
            id: cb;

            width: 300;
            height: 30;

            Component.onCompleted: {
                root.setupComboBox(cb);
            }
        }
    }
}


