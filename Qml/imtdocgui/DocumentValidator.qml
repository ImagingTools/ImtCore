import QtQuick 2.12
import Acf 1.0


/*
    Usage example

    function isValid(data){
        if (!documentModel){
            data.message = qsTr("Document model is invalid")
            return false;
        }

        let deviceType = documentModel.m_deviceType;
        if (String(deviceType) === ""){
            data.message = qsTr("Please select a device type")

            return false;
        }

        return true;
    }
*/

QtObject {
    id: root;

    property var documentModel: null;

    function isValid(data){
        return true;
    }
}
