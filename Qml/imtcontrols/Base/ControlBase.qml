import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


FocusScope {
    id: controlBase;
    
    width: decorator ? decorator.width : 0;
    height: decorator ? decorator.height : 0;

    property Component decoratorComponent;
    property var decorator : null;

    onDecoratorComponentChanged: {
        decoratorChangedFunc();
    }

    function decoratorChangedFunc(){
        decoratorChangedFuncBase();
        setBindTargets();
    }

    function decoratorChangedFuncBase(){
        if(!decoratorComponent){
            return;
        }
        if(decorator){
            decorator.destroy();
        }
        decorator = decoratorComponent.createObject(controlBase);
        decorator.baseElement = controlBase;
    }

    function setBindTargets(){
        bindWidth.target = decorator;
        bindHeight.target = decorator;
    }

    Binding {
        id: bindWidth
        property: "width"
        value: controlBase.width;
    }

    Binding {
        id: bindHeight
        property: "height"
        value: controlBase.height;
    }
}
