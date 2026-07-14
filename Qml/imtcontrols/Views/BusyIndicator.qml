import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ControlBase{
    id: busyIndicator;
    // Test instrumentation: lets GUI tests wait for this to disappear before screenshotting - its
    // rotation animation (in the decorator) never touches objectName/visible/childList, so a
    // MutationObserver-based "DOM went quiet" wait can resolve while it's still visibly spinning.
    // Inert - objectName has no runtime/visual effect.
    objectName: "BusyIndicator";

    decorator: Style.busyIndicatorDecorator;

    function start(){
        stop();

        visible = true;
    }

    function stop(){
        visible = false;
    }
}

