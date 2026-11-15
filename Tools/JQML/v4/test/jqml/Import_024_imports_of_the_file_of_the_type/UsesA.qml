import QtQuick
import ModA 1.0

Item {
    property string thingName: thing.name
    property string styleName: Style.name

    Thing { id: thing }
}
