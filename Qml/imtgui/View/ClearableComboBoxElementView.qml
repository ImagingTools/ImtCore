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
            width: root.controlWidth;
            height: root.controlHeight;

            onFocusChanged: {
                console.log("ClearableComboBoxElementView.qml onFocusChanged",focus);

                if (focus){
                    cb.forceActiveFocus();
                }
            }

            ComboBox {
                id: cb;

                anchors.left: parent.left;
                anchors.right: clearButton.left;
                anchors.rightMargin: Style.size_mainMargin

                height: 30;

                Component.onCompleted: {
                    root.setupComboBox(cb);
                }

                onFocusChanged: {
                    console.log("ComboBox onFocusChanged",focus);
                }
            }

            Button {
                id: clearButton;

                anchors.right: parent.right;

                height: 30;

                widthFromDecorator: true;

                text: qsTr("Clear");

                enabled: root.changeable && root.currentIndex >= 0;

                onClicked: {
                    root.currentIndex = -1;
                }
            }
        }
    }
}


