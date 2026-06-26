import QtQuick
import "./Decorators"

Item {
    TestComponent {
        id: testComponent

        Component.onCompleted: {
            console.log("TestComponent.onCompleted")
        }
    }
}