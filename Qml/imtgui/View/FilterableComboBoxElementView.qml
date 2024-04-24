import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBoxElementView {
    id: root;

    controlComp: cbComp;

    Component {
        id: cbComp;

        Item {
            width: 300;
            height: 30;

            onFocusChanged: {
                if (focus){
                    cb.forceActiveFocus();
                }
            }

            FilterableComboBox {
                id: cb;

                anchors.left: parent.left;
                anchors.right: clearButton.left;
                anchors.rightMargin: Style.size_mainMargin

                width: 230;
                height: 30;

                Component.onCompleted: {
                    root.setupComboBox(cb);
                }
            }

            Button {
                id: clearButton;

                anchors.right: parent.right;

                height: 30;

                text: qsTr("Clear");

                widthFromDecorator: true;

                enabled: root.changeable && root.currentIndex >= 0;

                onClicked: {
                    root.currentIndex = -1;
                }
            }
        }
    }
}


