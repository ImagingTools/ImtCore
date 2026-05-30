import QtQuick

Item {
    id: myitem

    signal mySignal(text: string, number: int)
    signal mySignal2(text: string, number: int)

    onMySignal: (text, number) => console.log(text, number)

    onMySignal2: function(text, number) {
        console.log(text, number)
    }

    function testFunc(){
        let arrowFunc = ()=>{
            console.log('arrowFunc called')
        }

        arrowFunc()
        mySignal('Hello', 555)
        mySignal2('World', 777)
    }

    Component.onCompleted: {
        testFunc()
        Qt.quit()
    }
}
