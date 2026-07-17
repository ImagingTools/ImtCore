pragma Singleton
import QtQuick 2.12

QtObject {
    property int ok: 1024
    property int yes: 16384
    property int open: 8192
    property int save: 2048
    property int cancel: 4194304
    property int close: 2097152
    property int discard: 8388608
    property int apply: 33554432
    property int reset: 67108864
    property int restoreDefaults: 134217728
    property int help: 16777216
    property int saveAll: 4096
    property int yesToAll: 32768
    property int no: 65536
    property int noToAll: 131072
    property int abort: 262144
    property int retry: 524288
    property int ignore: 1048576
    property int noButton: 0

    // -------------------------------------------------------------------- //
    // imtcontrols.Popup close-policy bit flags. Combine with |.
    // Mirrors Qt Quick Controls Popup.ClosePolicy.
    // -------------------------------------------------------------------- //
    readonly property int popupNoAutoClose:                  0x00
    readonly property int popupCloseOnPressOutside:          0x01
    readonly property int popupCloseOnPressOutsideParent:    0x02
    readonly property int popupCloseOnReleaseOutside:        0x04
    readonly property int popupCloseOnReleaseOutsideParent:  0x08
    readonly property int popupCloseOnEscape:                0x10
    readonly property int popupDefaultClosePolicy:           popupCloseOnEscape | popupCloseOnPressOutside

    enum PopupClosePolicy {
        NoAutoClose                  = 0x00,
        CloseOnPressOutside          = 0x01,
        CloseOnPressOutsideParent    = 0x02,
        CloseOnReleaseOutside        = 0x04,
        CloseOnReleaseOutsideParent  = 0x08,
        CloseOnEscape                = 0x10
    }

    // -------------------------------------------------------------------- //
    // imtguigql.FileSystemBrowserDialog / ServerPathPicker selectable-kind
    // bit flags. Combine with | (e.g. pathKindFile | pathKindDir to allow
    // picking either). Mirrors the popupCloseOn* bit-flag style above.
    // -------------------------------------------------------------------- //
    readonly property int pathKindFile: 0x01
    readonly property int pathKindDir:  0x02

    enum PathKind {
        File = 0x01,
        Dir  = 0x02
    }

    enum ButtonType {
        Ok = 1024,
        Open = 8192,
        Save = 2048,
        Cancel = 4194304,
        Close = 2097152,
        Discard = 8388608,
        Apply = 33554432,
        Reset = 67108864,
        RestoreDefaults = 134217728,
        Help = 16777216,
        SaveAll = 4096,
        Yes = 16384,
        YesToAll = 32768,
        No = 65536,
        NoToAll = 131072,
        Abort = 262144,
        Retry = 524288,
        Ignore = 1048576,
        NoButton = 0
    }
}
