const { Item } = require('./Item')
const { QSignal } = require('../utils/signal')
const { Qt } = require('../utils/Qt')
const { QReal, QBool, QString, QInt, QPoint, QVar } = require('../utils/properties')

class WheelHandler extends Item {
    static defaultProperties = {
        acceptedButtons: { type: QInt, value: Qt.LeftButton },
        acceptedDevices: { type: QInt, value: Qt.LeftButton },
        acceptedModifiers: { type: QInt, value: Qt.LeftButton },
        acceptedPointerTypes: { type: QInt, value: Qt.LeftButton },
        active: { type: QBool, value: true },
        activeTimeout: { type: QReal, value: 0 },
        blocking: { type: QBool, value: false },
        cursorShape: { type: QString, value: 'default' },
        dragThreshold: { type: QInt, value: 0 },
        grabPermissions: { type: QInt, value: 0 },
        invertible: { type: QBool, value: false },
        margin: { type: QReal, value: 0 },
        orientation: { type: QReal, value: 0 },
        point: { type: QPoint, value: undefined },
        property: { type: QString, value: '' },
        rotation: { type: QReal, value: 0 },
        rotationScale: { type: QReal, value: 0 },
        target: { type: QVar, value: undefined },
        targetScaleMultiplier: { type: QReal, value: 0 },
        targetTransformAroundCursor: { type: QBool, value: false },

    }

    static defaultSignals = {
        canceled: { params: ['point'] },
		grabChanged: { params: ['transition', 'point'] },
		wheel: { params: ['event'] },

    }


}

module.exports.WheelHandler = WheelHandler