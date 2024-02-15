import QtQuick 2.12
import Acf 1.0

Item {
    id: container;

    property Item view: null;

    signal commandActivated(string commandId);

    function commandHandle(commandId){
        commandActivated(commandId);
    }
}
