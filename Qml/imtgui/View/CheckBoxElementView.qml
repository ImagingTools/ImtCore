import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: checkBoxElementView;

    controlComp: checkBoxComp;

    property int checkState: Qt.Unchecked;

    property string text;

    signal clicked();

    onClicked: {
        if (checkState == Qt.Unchecked){
            checkState = Qt.Checked;
        }
        else{
            checkState = Qt.Unchecked;
        }
    }

    Component {
        id: checkBoxComp;

        CheckBox {
            id: cb;
            width: checkBoxElementView.controlWidth;
            height: checkBoxElementView.controlHeight;
            checkState: checkBoxElementView.checkState;
            text: checkBoxElementView.text;

			function nextCheckState() {
                checkBoxElementView.clicked();
            }
        }
    }
}


