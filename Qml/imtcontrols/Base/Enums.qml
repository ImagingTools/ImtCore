pragma Singleton
import QtQuick 2.12

QtObject {
    readonly property int ok: 1024
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

    function test (){

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
