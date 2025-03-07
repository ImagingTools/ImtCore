import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: textElementView;

    controlComp: textComp;

    property string text;

    property Text textRef;

    Component {
        id: textComp;

        Text {
            id: txt;

            color: Style.textColor;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeNormal;
            text: textElementView.text;

            Component.onCompleted: {
                textElementView.textRef = txt;
            }
        }
    }
}


