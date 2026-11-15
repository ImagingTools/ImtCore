import QtQuick

QtObject {
    property int bound: ModuleType.Kind.Second
    property ModuleType type: ModuleType {}

    function read(){
        return ModuleType.Third + type.ownKind()
    }
}
