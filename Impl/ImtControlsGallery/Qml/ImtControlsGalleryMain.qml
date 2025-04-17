import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguivars 1.0

ApplicationMain{
    id: window;

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
        firstModelsInit();
    }

    function updateAllModels(){
        decorators.topPanelDecorator = topPanelDecoratorCustom
        Style.setDecorators(decorators)
        Events.subscribeEvent("OnStyleDecoratorChanged", window.onStyleDecoratorChanged);

        thumbnailDecoratorGui.updateModels();

        applicationInfoProvider.updateModel();
    }

    function onStyleDecoratorChanged(decoratorId){
        if (decoratorId == "StyleQt"){
            Style.setDecorators(decoratorsQt)
        }
        else{
            Style.setDecorators(decorators)
        }
    }
}


