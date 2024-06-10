import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBoxElementView {
    id: root;

    controlComp: cbComp;

    property var filteringFields: [nameId];
    property string descriptionFieldId;

    Component {
        id: cbComp;

        Item {
            width: root.controlWidth;
            height: root.controlHeight;

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

                height: 30;

                filteringFields: root.filteringFields;
                descriptionFieldId: root.descriptionFieldId;

                Component.onCompleted: {
                    if (root.delegate){
                        cb.delegate = root.delegate;
                    }

                    root.setupComboBox(cb);
                }
            }

            Button {
                id: clearButton;

                anchors.right: parent.right;

                height: 30;

                text: qsTr("clear");

                widthFromDecorator: true;

                enabled: root.changeable && root.currentIndex >= 0;

                onClicked: {
                    root.currentIndex = -1;
                }
            }
        }
    }
}


