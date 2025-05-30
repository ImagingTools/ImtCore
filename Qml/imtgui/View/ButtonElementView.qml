import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: elementView;
    property string text;
    property Button button;
    property bool buttonEnabled: true;
    signal clicked();

    controlComp: Component {
        Button {
            id: buttonControl;
			width: Style.sizeHintBXS;
			height: Style.controlHeightM;
            text: elementView.text;
            enabled: elementView.buttonEnabled;
            Component.onCompleted: {
                elementView.button = buttonControl
            }
            onClicked: {
                elementView.clicked();
            }
        }
    }
}


