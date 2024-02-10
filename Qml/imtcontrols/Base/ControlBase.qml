import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


FocusScope {
    id: controlBase;
    
    width: decorator_ ? decorator_.width : 0;
    height: decorator_ ? decorator_.height : 0;

    property Component decorator;
    property var decorator_ : null;

    property alias bindWidthAlias: bindWidth;
    property alias bindHeightAlias: bindHeight;

    property bool widthFromDecorator: false;
    property bool heightFromDecorator: false;

    onDecoratorChanged: {
        decoratorChangedFunc();
    }

    function decoratorChangedFunc(){
        decoratorChangedFuncBase();
        setBindTargets();
    }

    function decoratorChangedFuncBase(){
        if(decorator == null || decorator == undefined){
            return;
        }
        if(decorator_){
            decorator_.destroy();
        }
        decorator_ = decorator.createObject(controlBase);
        decorator_.baseElement = controlBase;
    }

    function setBindTargets(){
        if (!widthFromDecorator){
            bindWidth.target = decorator_;
        }

        if (!heightFromDecorator){
            bindHeight.target = decorator_;
        }
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


