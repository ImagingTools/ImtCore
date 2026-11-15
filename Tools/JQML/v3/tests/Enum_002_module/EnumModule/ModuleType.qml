import QtQuick

QtObject {
    enum Kind { First, Second, Third = 10 }

    function ownKind(){
        return ModuleType.Kind.Second
    }
}
