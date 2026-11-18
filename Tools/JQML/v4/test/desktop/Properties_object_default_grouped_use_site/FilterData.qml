import QtQuick

QtObject {
    property string fieldId: ""
    property string valueType: "String"
    property var operations: ["Equal"]
    function copy() { return { fieldId: fieldId, valueType: valueType, operations: operations } }
}
