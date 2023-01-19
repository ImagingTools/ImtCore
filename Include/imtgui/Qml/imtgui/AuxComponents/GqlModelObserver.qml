import QtQuick 2.15
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property GqlModel observedModel: null;
    property Item noConnectionView: null;

    Component.onDestruction: {
        if (container.observedModel != null){
            container.observedModel.stateChanged.disconnect(container.observedModelStateChanged);
        }
    }

    onObservedModelChanged: {
        if (container.observedModel != null){
            container.observedModel.stateChanged.connect(container.observedModelStateChanged);
        }
    }

    function observedModelStateChanged(){
        if (container.noConnectionView != null){
            if (container.observedModel.state == "Ready"){
                container.noConnectionView.visible = false;
            }
            else if (container.observedModel.state == "Error"){
                container.noConnectionView.visible = true;
            }
        }
    }
}
