import QtQuick
import prolifeSensorsSdl 1.0
import prolifeOrdersSdl 1.0
import prolifeAccountsSdl 1.0

Item {
    DeviceDataInput { id: device }
    OrderDataInput { id: order }
    AccountDataInput { id: account }

    Component.onCompleted: {
        account.m_name = "Customer-A"
        order.m_orderNumber = "ORD-42"
        device.m_name = "Sensor-7"

        console.log(
            "prolife11",
            account.__typename,
            order.__typename,
            device.__typename,
            ProlifeAccountsSdlCommandIds.s_accountsList,
            ProlifeOrdersSdlCommandIds.s_orderAdd,
            ProlifeSensorsSdlCommandIds.s_deviceAdd
        )
        Qt.quit()
    }
}
