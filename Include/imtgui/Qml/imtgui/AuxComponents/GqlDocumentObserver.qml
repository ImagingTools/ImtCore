import QtQuick 2.15
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property GqlModel observedGetModel: null;
    property GqlModel observedSetModel: null;

    property Item loadingView: null;

    Component.onDestruction: {
        if (container.observedGetModel){
            container.observedGetModel.stateChanged.disconnect(container.observedModelStateChanged);
        }

        if (container.observedSetModel){
            container.observedSetModel.stateChanged.disconnect(container.observedModelStateChanged);
        }
    }

    onObservedGetModelChanged: {
        if (container.observedGetModel){
            container.observedGetModel.stateChanged.connect(container.observedModelStateChanged);
        }
    }

    onObservedSetModelChanged: {
        if (container.observedSetModel){
//            container.observedSetModel.stateChanged.connect(container.observedModelStateChanged);
        }
    }

    function observedModelStateChanged(){
        if (container.loadingView != null){
            if (container.observedGetModel.state === "Loading"){
                container.loadingView.visible = true;
            }
            else{
                container.loadingView.visible = false;
            }
        }
    }
}
