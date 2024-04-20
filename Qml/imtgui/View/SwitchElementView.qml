import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: switchElementView;

    controlComp: switchComp;

    property bool checked: false;

    property SwitchCustom switchRef;

    property bool readOnly: false;

    onCheckedChanged: {
        if (switchRef){
            switchRef.checked = checked;
        }
    }

    onReadOnlyChanged: {
        if (switchRef){
            switchRef.readOnly = readOnly;
        }
    }

    Component {
        id: switchComp;

        SwitchCustom {
            id: cb;

            checked: switchElementView.checked;

            onCheckedChanged: {
                switchElementView.checked = checked;
            }

            Component.onCompleted: {
                switchElementView.switchRef = cb;
                cb.readOnly = switchElementView.readOnly;
            }
        }
    }
}


