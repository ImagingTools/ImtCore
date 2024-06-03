import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0

ComboBoxElementView {
    id: root;

    property string gqlCommandId;
    property var fields;

    controlComp: cbComp;

    function getAdditionalInputParams(){
        return {};
    }

    Component {
        id: cbComp;

        ComboBoxGqlSimple {
            id: cb;

            width: root.controlWidth;
            height: root.controlHeight;

            gqlCommandId: root.gqlCommandId;
            fields: root.fields;

            Component.onCompleted: {
                if (root.delegate){
                    cb.delegate = root.delegate;
                }

                root.setupComboBox(cb);
            }

            function getAdditionalInputParams(){
                return root.getAdditionalInputParams();
            }
        }
    }
}
