import QtQuick

// An id of a Component and a property of the same name in its item: the id is the component outside, the property
// inside (the component has an id scope of its own)
Item {
    id: app
    Component {
        id: number
        Rectangle {
            id: inner
            property int number: 2
            property int twice: number * 2
        }
    }
    function make() { return number.createObject(app, { number: 5 }) }
    Component.onCompleted: {
        let o = make()
        console.log(typeof number.createObject, o.number, o.twice, typeof inner)
        Qt.quit()
    }
}
