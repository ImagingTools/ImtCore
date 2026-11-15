import QtQuick
import imtcontrols 1.0

Item {
    Component.onCompleted: {
        console.log("button-type", Enums.ButtonType.Ok, Enums.ButtonType.Save, Enums.Cancel)
        console.log("close-policy", Enums.PopupClosePolicy.CloseOnEscape, Enums.CloseOnPressOutside)
        console.log("path-kind", Enums.PathKind.Dir, Enums.File)
        console.log("properties", Enums.save, Enums.popupCloseOnEscape, Enums.pathKindDir)
        Qt.quit()
    }
}
