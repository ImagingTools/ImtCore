import QtQuick

// a type of a project named as the one Qt 6.10 added to QtQml.Models (imtcontrols has one)
QtObject {
    property string textFilter
    property var filterableFields: []
    signal accepted()
    function apply(){ accepted() }
}
