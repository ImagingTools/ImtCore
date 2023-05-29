import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

QtObject {
    id: container;

    property GqlModel observedModel: null;
    property Item noConnectionView: null;

    Component.onDestruction: {
        if (container.observedModel){
            container.observedModel.stateChanged.disconnect(container.onStateChanged);
        }
    }

    onObservedModelChanged: {
        if (container.observedModel){
            container.observedModel.stateChanged.connect(container.onStateChanged);
        }
    }

    function onStateChanged(){
        if (container.noConnectionView != null){
            if (container.observedModel.state === "Ready"){
                container.noConnectionView.visible = false;
            }
            else if (container.observedModel.state === "Error"){
                container.noConnectionView.visible = true;
            }
        }
    }
}
