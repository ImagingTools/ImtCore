import QtQuick

// The enums of a QML type are read through the name of the type: `Source.Kind.Own` and `Source.Own`, inside of the file of the type
// as well (DocumentRepresentationController.isUpdateRequired of ImtCore compares with its own enum).
QtObject {
    enum Kind {
        Own,
        View,
        External
    }

    enum Level {
        Low = 10,
        High = 20
    }

    function isOwn(kind) {
        return kind === Source.Kind.Own
    }

    function isHigh(level) {
        return level === Source.High
    }
}
