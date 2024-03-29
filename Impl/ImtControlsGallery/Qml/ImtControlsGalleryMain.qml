import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguivars 1.0

ApplicationMain{
    id: window;

    ModalDialogManager {
        id: modalDialogManager;

        z: 30;

        anchors.fill: parent;
    }

    Component {
        id: topPanelDecoratorCustom
        TopPanelDecoratorCustom {

        }
    }

    Decorators {
        id: decorators
    }

    DecoratorsQt {
        id: decoratorsQt
    }

    Component.onCompleted: {
        console.log("MainOnCompleted", topPanelDecoratorCustom)
//        console.log("IconMode.selected", IconMode.selected)
    }

    onServerReadyChanged: {
        firstModelsInit();
    }

    function updateAllModels(){
        console.log("settingsProviderLocal.updateModel", Style.textColor);
        decorators.topPanelDecorator = topPanelDecoratorCustom
        Style.setDecorators(decorators)
        Events.subscribeEvent("OnStyleDecoratorChanged", window.onStyleDecoratorChanged);

        thumbnailDecoratorGui.updateModels();

        applicationInfoProvider.updateModel();
    }

    function onStyleDecoratorChanged(decoratorId){
        console.log("onStyleDecoratorChanged", decoratorId);
        if (decoratorId == "StyleQt"){
            Style.setDecorators(decoratorsQt)
        }
        else{
            Style.setDecorators(decorators)
        }
    }
}


