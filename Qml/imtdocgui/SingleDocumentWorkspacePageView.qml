import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtgui 1.0

DocumentWorkspacePageView {
    id: root;

    documentManagerView: documentManagerView;

    onStartItemSourceCompChanged: {
        if (startItemSourceComp){
            documentManagerView.addInitialItem(startItemSourceComp, pageName);
        }
    }

    SingleDocumentWorkspaceView {
        id: documentManagerView;
        anchors.fill: parent;
        documentManager: root.documentManager;

        Component.onCompleted: {
            Events.subscribeEvent("SetAlertPanel", setAlert);
        }

        Component.onDestruction: {
            Events.unSubscribeEvent("SetAlertPanel", setAlert);
        }

        function setAlert(parameters){
            if (!parameters){
                console.error("Unable to set alert panel, 'parameters' is invalid");
                return;
            }

            let id = parameters["Id"];
            let alertPanelComp = parameters["AlertPanelComp"];

            if (id === root.pageId){
                setAlertPanel(alertPanelComp);
            }
        }
    }
}
