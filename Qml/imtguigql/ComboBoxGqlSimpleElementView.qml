import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0

ComboBoxElementView {
    id: root;

    property string gqlCommandId;
    property string subscriptionCommandId;
    property var fields;
    property var filter;

    controlComp: cbComp;

    function getHeaders(){
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
            subscriptionCommandId: root.subscriptionCommandId;
            filter: root.filter;

            Component.onCompleted: {
                if (root.delegate){
                    cb.delegate = root.delegate;
                }

                root.setupComboBox(cb);
            }

            onModelChanged: root.model = model;

            function getHeaders(){
                return root.getHeaders();
            }

			Connections {
				target: root;

				function onModelChanged(){
					if (cb.model != root.model){
						cb.model = root.model;
					}
				}

				function onChangeableChanged(){
					if (cb.changeable != root.changeable){
						cb.changeable = root.changeable;
					}
				}

				function onCurrentIndexChanged(){
					if (cb.currentIndex != root.currentIndex){
						cb.currentIndex = root.currentIndex;
					}
				}
			}
        }
    }
}
