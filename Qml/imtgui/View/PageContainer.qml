import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    property string pageId;
    property string pageName;
    property string startItemSource;
    property Component startItemSourceComp;

    function createStartItemComp(){
        startItemSourceComp = Qt.createComponent(startItemSource)
        if (startItemSourceComp && startItemSourceComp.status !== Component.Ready){
            console.error("Unable to create start item component. Error: ", startItemSourceComp.errorString());
        }
    }

    function startItemIsCreated(){
        return startItemSourceComp != null && startItemSourceComp !== undefined;
    }
}

