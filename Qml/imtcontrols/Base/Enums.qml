import QtQuick 2.12
//import QtQuick.Controls 2.15

QtObject {

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
