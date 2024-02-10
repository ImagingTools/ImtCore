import QtQuick 2.12
import Acf 1.0

QtObject {
    id: container;

    property DocumentManager documentManager: null;

    property Component documentHistoryDialogComp: Component {
        DocumentHistoryDialog {
            title: qsTr("Document history");

            onStarted: {
                updateModel();
            }
        }
    }

    signal commandActivated(string commandId);

    function commandHandle(commandId){
        console.log("commandHandle", commandId, container);

        commandActivated(commandId);
    }
}
