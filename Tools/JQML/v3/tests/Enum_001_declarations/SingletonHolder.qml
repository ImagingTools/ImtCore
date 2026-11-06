pragma Singleton
import QtQuick

QtObject {
    enum Kind { Own, View = 5, External }
    enum Explicit { First = 1, Second = 2 }
    readonly property int plainValue: 7
}
