import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ControlBase {
    id: tabDelegate;

    property bool selected: false;
    property bool firstElement: false;
    property bool lastElement: false;
    property bool autoWidth: true;
    property bool isCloseEnable: true;

    property string text;
    property string firstElementImageSource:  "../../../" + "Icons/" + Style.theme + "/" + "Workflow" + "_On_Normal.svg";

    decoratorComponent: Style.tabPanelDecorator

    signal clicked;
    signal closeSignal;

    function setBindTargets(){
        bindWidthAlias.value = decorator.width;
        bindWidthAlias.property = "width";
        bindWidthAlias.target = tabDelegate;

        bindHeightAlias.target = decorator;
    }

    MouseArea{
        id: ma;

        anchors.fill: tabDelegate;

        onClicked: {
            console.log("tabDelegate clicked ");
            tabDelegate.clicked();
        }
    }
}
