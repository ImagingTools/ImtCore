const { Popup } = require('./Popup')
const { QBool, QReal, QInt, QVar } = require('../utils/properties')

/**
 * Drawer — Qt Quick Controls Drawer implementation for JQML v2
 * https://doc.qt.io/qt-6/qml-qtquick-controls-drawer.html
 *
 * A side-panel popup that slides in from an edge of the window.
 * Supports edge positioning (Qt.LeftEdge, Qt.RightEdge, Qt.TopEdge, Qt.BottomEdge),
 * interactive swipe, and position/dragMargin properties.
 */

// Qt edge constants
const Edge = {
    LeftEdge: 1,
    RightEdge: 2,
    TopEdge: 4,
    BottomEdge: 8,
}

class Drawer extends Popup {
    static LeftEdge = Edge.LeftEdge
    static RightEdge = Edge.RightEdge
    static TopEdge = Edge.TopEdge
    static BottomEdge = Edge.BottomEdge

    static defaultProperties = {
        edge: { type: QInt, value: Edge.LeftEdge, changed: '$edgeChanged' },
        position: { type: QReal, value: 0, changed: '$positionChanged' },
        interactive: { type: QBool, value: true },
        dragMargin: { type: QReal, value: 20 },
    }

    static defaultSignals = {}

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        // Override modal default for drawers
        this.getProperty('modal').reset(true)
        this.getProperty('dim').reset(true)

        // Override content box styles for drawer behavior
        this.$contentBox.style.borderRadius = '0'
        this.$contentBox.style.transition = 'transform 0.3s ease'

        this.$edgeChanged()
    }

    open(){
        super.open()
        this.getProperty('position').reset(1.0)
        this.$applyEdgeTransform(1.0)
    }

    close(){
        this.getProperty('position').reset(0)
        this.$applyEdgeTransform(0)
        // Delay hiding until transition finishes
        setTimeout(() => {
            super.close()
        }, 300)
    }

    $edgeChanged(){
        let edge = this.getPropertyValue('edge')
        let box = this.$contentBox
        // Reset all edge positioning
        box.style.left = ''
        box.style.right = ''
        box.style.top = ''
        box.style.bottom = ''
        box.style.width = ''
        box.style.height = ''

        switch (edge){
            case Edge.LeftEdge:
                box.style.left = '0'
                box.style.top = '0'
                box.style.bottom = '0'
                box.style.width = '280px'
                box.style.height = '100%'
                break
            case Edge.RightEdge:
                box.style.right = '0'
                box.style.top = '0'
                box.style.bottom = '0'
                box.style.width = '280px'
                box.style.height = '100%'
                break
            case Edge.TopEdge:
                box.style.left = '0'
                box.style.right = '0'
                box.style.top = '0'
                box.style.width = '100%'
                box.style.height = '200px'
                break
            case Edge.BottomEdge:
                box.style.left = '0'
                box.style.right = '0'
                box.style.bottom = '0'
                box.style.width = '100%'
                box.style.height = '200px'
                break
        }

        this.$applyEdgeTransform(this.getPropertyValue('position'))
    }

    $positionChanged(){
        this.$applyEdgeTransform(this.getPropertyValue('position'))
    }

    $applyEdgeTransform(position){
        let edge = this.getPropertyValue('edge')
        let box = this.$contentBox
        let offset = (1 - position) * 100

        switch (edge){
            case Edge.LeftEdge:
                box.style.transform = `translateX(-${offset}%)`
                break
            case Edge.RightEdge:
                box.style.transform = `translateX(${offset}%)`
                break
            case Edge.TopEdge:
                box.style.transform = `translateY(-${offset}%)`
                break
            case Edge.BottomEdge:
                box.style.transform = `translateY(${offset}%)`
                break
        }
    }
}

module.exports.Drawer = Drawer
