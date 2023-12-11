const { QSignal } = require('./signal')
const { ComplexObject } = require('./base')

class QProperty {
    completed = true
    
    constructor(value, type){
        if(type) this.type = type
        
        this.value = this.getDefaultValue()
        if(typeof value === 'function'){
            this.setCompute(value)
        } else if(value !== undefined){
            try {
                let safeValue = this.typeCasting(value)
                this.value = safeValue
            } catch (error) {
                console.error(error)
            }
            
        }
    }

    getDefaultValue(){
        return undefined
    }

    typeCasting(value){
        return value
    }

    update(){
        if(this.debug) console.log('updating...', this.id)
        if(this.updating || !this.compute) return
        this.updating = true
        global.queueLink.push(this)
        let value = this.value
        try {
            value = this.compute()
        } catch (error) {
            
        } finally {
            global.queueLink.pop()
        }
        
        if(this.compute) this.set(value)
        this.updating = false
        this.completed = true
    }

    get(){
        if(!this.completed){
            this.update()
        }   
        return this.value
    }

    getStatement(name){
        return this.get().getStatement(name)
    }

    set(newValue){
        let safeValue = this.value
        try {
            safeValue = this.typeCasting(newValue)
        } catch (error) {
            console.error(error)
        }
        if(safeValue !== this.value){
            this.value = safeValue
            if(this.notify) this.notify()
        }
    }

    reset(newValue){
        this.unsubscribe()
        this.set(newValue)
    }

    setCompute(compute){
        // updateList.push(this)
        this.compute = compute
        this.completed = false
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    connect(...args){
        this.getNotify().connect(...args)
    }

    addSubscriber(target){
        if(!this.subscribers) this.subscribers = []
        if(!this.notify) this.notify = new QSignal()

        if(this.subscribers.indexOf(target) < 0) {
            this.subscribers.push(target)
            this.notify.connect(target, target.update)
        }
    }

    removeSubscriber(target){
        if(!this.subscribers || !this.notify) return

        this.notify.disconnect(target, target.update)
        let index = this.subscribers.indexOf(target)
        if(index >= 0){
            this.subscribers.splice(index, 1)
            if(this.subscribers.length === 0) {
                delete this.subscribers
                if(!this.notify.connections || this.notify.connections.length === 0) delete this.notify
            }
        }
    }

    unsubscribe(){
        delete this.compute
        delete this.updating

        if(!this.depends) return
        
        while(this.depends.length){
            let target = this.depends.pop()
            target.removeSubscriber(this)
        }
        delete this.depends
    }

    subscribe(...targets){
        if(!this.depends) this.depends = []

        for(let target of targets){
            if(target && target.addSubscriber){
                if(this.depends.indexOf(target) < 0) {
                    this.depends.push(target)
                }
        
                target.addSubscriber(this)
            }
            
        }
        
    }
    subscribePrimary(...targets){
        if(!this.depends) this.depends = []

        for(let target of targets){
            if(this.depends.indexOf(target) < 0) {
                this.depends.push(target)
            }
    
            target.addSubscriberPrimary(this)
        }
        
    }
    addSubscriberPrimary(target){
        if(!this.subscribers) this.subscribers = []
        if(!this.notify) this.notify = new QSignal()

        if(this.subscribers.indexOf(target) < 0) {
            this.subscribers.push(target)
            this.notify.connectPrimary(target, target.update)
        }
    }

    destroy(){
        this.unsubscribe()
    }
}

class QString extends QProperty {
    getDefaultValue(){
        return ''
    }

    typeCasting(value){
        if(value === undefined) throw 'Cannot assign [undefined] to QString'
        if(typeof value === 'object') throw 'Cannot assign QJSValue to QString'
        if(typeof value === 'number') {
            if(isNaN(value)) return 'nan'
            if(value === Infinity) return 'inf'
            if(value === -Infinity) return '-inf'
            return value.toString()
        }
        if(value === null) return ''
        return value
    }
}

class QInt extends QProperty {
    getDefaultValue(){
        return 0
    }

    typeCasting(value){
        if(value === undefined) throw 'Cannot assign [undefined] to int'
        if(value === null) throw 'Cannot assign std::nullptr_t to int'
        if(typeof value === 'object') throw 'Cannot assign QJSValue to int'

        if(typeof value === 'number') {
            if(isNaN(value) || value === Infinity || value === -Infinity) return -2147483648
            return Number.parseInt(value)
        }

        if(value === true) {
            return 1
        }

        if(value === false) {
            return 0
        }

        if(typeof value === 'string') {
            let number = Number.parseInt(value)
            if(isNaN(number) || number === Infinity || number === -Infinity) throw 'Cannot assign QString to int'
            return number
        }
    }
}

class QReal extends QProperty {
    getDefaultValue(){
        return 0
    }

    typeCasting(value){
        if(value === undefined) throw 'Cannot assign [undefined] to double'
        if(value === null) throw 'Cannot assign std::nullptr_t to double'
        if(typeof value === 'object') throw 'Cannot assign QJSValue to double'

        if(typeof value === 'number') {
            return value
        }

        if(value === true) {
            return 1
        }

        if(value === false) {
            return 0
        }

        if(typeof value === 'string') {
            let number = Number.parseFloat(value)
            if(isNaN(number) || number === Infinity || number === -Infinity) throw 'Cannot assign QString to double'
            return number
        }
    }
}

class QPositiveReal extends QReal {
    typeCasting(value){
        let number = super.typeCasting(value)
        return number >= 0 ? number : 0
    }
}

class QColor extends QProperty {
    static colors = {
        "aliceblue":"#f0f8ff","antiquewhite":"#faebd7","aqua":"#00ffff","aquamarine":"#7fffd4","azure":"#f0ffff","beige":"#f5f5dc","bisque":"#ffe4c4","black":"#000000","blanchedalmond":"#ffebcd","blue":"#0000ff","blueviolet":"#8a2be2","brown":"#a52a2a","burlywood":"#deb887",
        "cadetblue":"#5f9ea0","chartreuse":"#7fff00","chocolate":"#d2691e","coral":"#ff7f50","cornflowerblue":"#6495ed","cornsilk":"#fff8dc","crimson":"#dc143c","cyan":"#00ffff",
        "darkblue":"#00008b","darkcyan":"#008b8b","darkgoldenrod":"#b8860b","darkgray":"#a9a9a9","darkgreen":"#006400","darkkhaki":"#bdb76b","darkmagenta":"#8b008b","darkolivegreen":"#556b2f",
        "darkorange":"#ff8c00","darkorchid":"#9932cc","darkred":"#8b0000","darksalmon":"#e9967a","darkseagreen":"#8fbc8f","darkslateblue":"#483d8b","darkslategray":"#2f4f4f","darkturquoise":"#00ced1",
        "darkviolet":"#9400d3","deeppink":"#ff1493","deepskyblue":"#00bfff","dimgray":"#696969","dodgerblue":"#1e90ff","firebrick":"#b22222","floralwhite":"#fffaf0","forestgreen":"#228b22","fuchsia":"#ff00ff",
        "gainsboro":"#dcdcdc","ghostwhite":"#f8f8ff","gold":"#ffd700","goldenrod":"#daa520","gray":"#808080","green":"#008000","greenyellow":"#adff2f","honeydew":"#f0fff0","hotpink":"#ff69b4","indianred ":"#cd5c5c","indigo":"#4b0082","ivory":"#fffff0","khaki":"#f0e68c",
        "lavender":"#e6e6fa","lavenderblush":"#fff0f5","lawngreen":"#7cfc00","lemonchiffon":"#fffacd","lightblue":"#add8e6","lightcoral":"#f08080","lightcyan":"#e0ffff","lightgoldenrodyellow":"#fafad2",
        "lightgrey":"#d3d3d3","lightgray":"#d3d3d3","lightgreen":"#90ee90","lightpink":"#ffb6c1","lightsalmon":"#ffa07a","lightseagreen":"#20b2aa","lightskyblue":"#87cefa","lightslategray":"#778899","lightsteelblue":"#b0c4de",
        "lightyellow":"#ffffe0","lime":"#00ff00","limegreen":"#32cd32","linen":"#faf0e6","magenta":"#ff00ff","maroon":"#800000","mediumaquamarine":"#66cdaa","mediumblue":"#0000cd","mediumorchid":"#ba55d3","mediumpurple":"#9370d8","mediumseagreen":"#3cb371","mediumslateblue":"#7b68ee",
        "mediumspringgreen":"#00fa9a","mediumturquoise":"#48d1cc","mediumvioletred":"#c71585","midnightblue":"#191970","mintcream":"#f5fffa","mistyrose":"#ffe4e1","moccasin":"#ffe4b5",
        "navajowhite":"#ffdead","navy":"#000080","oldlace":"#fdf5e6","olive":"#808000","olivedrab":"#6b8e23","orange":"#ffa500","orangered":"#ff4500","orchid":"#da70d6",
        "palegoldenrod":"#eee8aa","palegreen":"#98fb98","paleturquoise":"#afeeee","palevioletred":"#d87093","papayawhip":"#ffefd5","peachpuff":"#ffdab9","peru":"#cd853f","pink":"#ffc0cb","plum":"#dda0dd","powderblue":"#b0e0e6","purple":"#800080",
        "rebeccapurple":"#663399","red":"#ff0000","rosybrown":"#bc8f8f","royalblue":"#4169e1","saddlebrown":"#8b4513","salmon":"#fa8072","sandybrown":"#f4a460","seagreen":"#2e8b57","seashell":"#fff5ee","sienna":"#a0522d","silver":"#c0c0c0","skyblue":"#87ceeb","slateblue":"#6a5acd","slategray":"#708090","snow":"#fffafa","springgreen":"#00ff7f","steelblue":"#4682b4",
        "tan":"#d2b48c","teal":"#008080","thistle":"#d8bfd8","tomato":"#ff6347","turquoise":"#40e0d0","violet":"#ee82ee","wheat":"#f5deb3","white":"#ffffff","whitesmoke":"#f5f5f5","yellow":"#ffff00","yellowgreen":"#9acd32"
    }

    toRGBA(){
        if(!this.completed) this.update()
        if(!this.value){
            return {
                r: 255,
                g: 255,
                b: 255,
                a: 0,
            }
        }
        let hexColor = '#ffffff'
        let alpha = false
        if(QColor.colors[this.value]){
            hexColor = QColor.colors[this.value]
        } else {
            if(this.value[0] === '#') {
                if(this.value.length === 4){
                    hexColor = `#${this.value[1]}${this.value[1]}${this.value[2]}${this.value[2]}${this.value[3]}${this.value[3]}`
                } else if(this.value.length === 7) {
                    hexColor = this.value
                } else if(this.value.length === 5) {
                    alpha = true
                    hexColor = `#${this.value[1]}${this.value[1]}${this.value[2]}${this.value[2]}${this.value[3]}${this.value[3]}${this.value[4]}${this.value[4]}`
                } else if(this.value.length === 9) {
                    alpha = true
                    hexColor = this.value
                } else {
                    hexColor = this.value
                }
            } else {
                return {
                    r: 255,
                    g: 255,
                    b: 255,
                    a: 0,
                }
            }
        }
        if(alpha){
            return {
                r: parseInt(hexColor[3]+hexColor[4], 16),
                g: parseInt(hexColor[5]+hexColor[6], 16),
                b: parseInt(hexColor[7]+hexColor[8], 16),
                a: parseInt(hexColor[1]+hexColor[2], 16)/255,
            }
        } else {
            return {
                r: parseInt(hexColor[1]+hexColor[2], 16),
                g: parseInt(hexColor[3]+hexColor[4], 16),
                b: parseInt(hexColor[5]+hexColor[6], 16),
                a: 1,
            }
        }
        

    }

    getDefaultValue(){
        return '#000000'
    }
    
    typeCasting(value){
        return value
    }
}

class QBool extends QProperty {
    
}

class QVisible extends QBool {
    originValue = true

    update(){
        if(this.updating) return
        this.updating = true
        global.queueLink.push(this)
        let value = this.value
        try {
            value = this.compute ? this.originCompute() && this.compute() : this.originCompute() && this.originValue
        } catch (error) {
            
        } finally {
            global.queueLink.pop()
        }
        
        this.set(value)
        this.updating = false
        this.completed = true
    }

    setOriginCompute(originCompute){
        this.originCompute = originCompute
    }

    get(){
        if(!this.completed){
            this.update()
        }   
        return this.value
    }

    set(newValue){
        let safeValue = this.value
        try {
            safeValue = this.typeCasting(newValue)
        } catch (error) {
            console.error(error)
        }
        if(safeValue !== this.value){
            this.value = safeValue
            if(this.notify) this.notify()
        }
    }

    reset(newValue){
        try {
            let safeValue = this.typeCasting(newValue)
            this.originValue = safeValue
        } catch (error) {
            console.error(error)
        }
        
        this.unsubscribe()
        this.update()
    }
}

class QVar extends QProperty {
    
}

class QData extends QProperty {
    getNotify(){
        if(!this.notify) this.notify = new QSignal('topLeft', 'bottomRight', 'roles')
        return this.notify
    }
}

// class QFocus extends QBool {
//     getNotify(){
//         if(!this.notify) this.notify = new QSignal('topLeft', 'bottomRight', 'roles')
//         return this.notify
//     }
// }

class QAlias extends QProperty {
    once = true
    constructor(getTargetProperty){
        super()
        this.getTargetProperty = getTargetProperty   
    }
    get(){
        if(!this.completed){
            this.update()
        }   
        let targetProperty = this.getTargetProperty()
        if(targetProperty instanceof QProperty) {
            return targetProperty.get()
        } else {
            return targetProperty
        }
        
    }

    set(newValue){
        let targetProperty = this.getTargetProperty()
        let safeValue = targetProperty instanceof QProperty ? this.getTargetProperty().typeCasting.call(this, newValue) : newValue
        if(safeValue !== this.value || ((targetProperty instanceof QProperty) && safeValue !== targetProperty.value)){
            this.value = safeValue
            if(targetProperty instanceof QProperty) {
                if(this.compute){
                //     // targetProperty.update()
                //     if(targetProperty.compute){
                //         global.queueLink.push(targetProperty)
                //         try {
                //             targetProperty.compute()
                //         } catch (error) {
                            
                //         } finally {
                //             global.queueLink.pop()
                //         }   
                //     } 

                    targetProperty.updating = true
                }
                if(targetProperty.auto){
                    targetProperty.setAuto(safeValue)
                } else {
                    targetProperty.set(safeValue)
                }
                if(this.once){
                    delete this.once
                    if(!targetProperty.compute) return
                }
                
            }
            if(this.notify) this.notify()
        }
    }

    reset(newValue){
        this.unsubscribe()
        let targetProperty = this.getTargetProperty()
        let safeValue = targetProperty instanceof QProperty ? this.getTargetProperty().typeCasting.call(this, newValue) : newValue
        if(safeValue !== this.value){
            this.value = safeValue
            if(targetProperty instanceof QProperty) targetProperty.reset(safeValue)
            if(this.notify) this.notify()
        }
    }

    update(){
        if(this.updating) return
        this.updating = true
        global.queueLink.push(this)
        let value = this.value
        
        let targetProperty = this.getTargetProperty()
        if(targetProperty instanceof QProperty) this.subscribe(targetProperty)

        try {
            value = this.compute ? this.compute() : this.getTargetProperty().get()
        } catch (error) {
            
        } finally {
            global.queueLink.pop()
        }
        
        this.set(value)
        this.updating = false
        this.completed = true
    }
}

class QAnchors extends ComplexObject {
    static defaultProperties = {
        fill: { type: QVar, value: undefined, changed: 'mainChanged' },
        centerIn: { type: QVar, value: undefined, changed: 'mainChanged' },
        left: { type: QVar, value: undefined, changed: 'mainChanged' },
        right: { type: QVar, value: undefined, changed: 'mainChanged' },
        top: { type: QVar, value: undefined, changed: 'mainChanged' },
        bottom: { type: QVar, value: undefined, changed: 'mainChanged' },
        horizontalCenter: { type: QVar, value: undefined, changed: 'mainChanged' },
        verticalCenter: { type: QVar, value: undefined, changed: 'mainChanged' },
        leftMargin: { type: QReal, value: 0, changed: 'mainChanged' },
        topMargin: { type: QReal, value: 0, changed: 'mainChanged' },
        rightMargin: { type: QReal, value: 0, changed: 'mainChanged' },
        bottomMargin: { type: QReal, value: 0, changed: 'mainChanged' },
        margins: { type: QReal, value: 0, changed: 'marginsChanged' },
        horizontalCenterOffset: { type: QReal, value: 0, changed: 'mainChanged' },
        verticalCenterOffset: { type: QReal, value: 0, changed: 'mainChanged' },
    }

    mainChanged(){
        if(this.completed) this.getNotify()()
    }

    marginsChanged(){
        if(this.completed) {
            this.getStatement('leftMargin').reset(this.getStatement('margins').get())
            this.getStatement('rightMargin').reset(this.getStatement('margins').get())
            this.getStatement('topMargin').reset(this.getStatement('margins').get())
            this.getStatement('bottomMargin').reset(this.getStatement('margins').get())
        }
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true
        
        if(this.$properties.fill) this.$properties.fill.update()
        if(this.$properties.centerIn) this.$properties.centerIn.update()
        if(this.$properties.leftMargin) this.$properties.leftMargin.update()
        if(this.$properties.rightMargin) this.$properties.rightMargin.update()
        if(this.$properties.topMargin) this.$properties.topMargin.update()
        if(this.$properties.bottomMargin) this.$properties.bottomMargin.update()
        if(this.$properties.left) this.$properties.left.update()
        if(this.$properties.right) this.$properties.right.update()
        if(this.$properties.top) this.$properties.top.update()
        if(this.$properties.bottom) this.$properties.bottom.update()
        if(this.$properties.horizontalCenter) this.$properties.horizontalCenter.update()
        if(this.$properties.verticalCenter) this.$properties.verticalCenter.update()
        if(this.$properties.horizontalCenterOffset) this.$properties.horizontalCenterOffset.update()
        if(this.$properties.verticalCenterOffset) this.$properties.verticalCenterOffset.update()
        if(this.$properties.margins) this.$properties.margins.update()
    }
}
class QAnchorLine {
    static Left = 0
    static Right = 1
    static Top = 2
    static Bottom = 3
    static HorizontalCenter = 4
    static VerticalCenter = 5

    constructor(target, float){
        this.target = target
        this.float = float
    }

    get(){
        return this
    }
}

class QFont extends ComplexObject {
    static defaultProperties = {
        bold: { type: QBool, value: false, changed: 'mainChanged' },
        family: { type: QString, value: '', changed: 'mainChanged' },
        italic: { type: QBool, value: false, changed: 'mainChanged' },
        underline: { type: QBool, value: false, changed: 'mainChanged' },
        pointSize: { type: QReal, value: 14/0.75, changed: 'mainChanged' },
        pixelSize: { type: QReal, value: 14, changed: 'mainChanged' },
    }

    mainChanged(){
        this.getNotify()()
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true

        if(this.$properties.bold) this.$properties.bold.update()
        if(this.$properties.family) this.$properties.family.update()
        if(this.$properties.italic) this.$properties.italic.update()
        if(this.$properties.underline) this.$properties.underline.update()
        if(this.$properties.pointSize) this.$properties.pointSize.update()
        if(this.$properties.pixelSize) this.$properties.pixelSize.update()

    }
}

class QBorder extends ComplexObject {
    static defaultProperties = {
        color: { type: QColor, value: 'black', changed: 'mainChanged' },
        width: { type: QReal, value: 0, changed: 'mainChanged' },
    }

    mainChanged(){
        this.getNotify()()
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true

        if(this.$properties.width) this.$properties.width.update()
        if(this.$properties.color) this.$properties.color.update()
    }
}

class QLine extends ComplexObject {
    static defaultProperties = {
        color: { type: QColor, value: 'black', changed: 'mainChanged' },
        width: { type: QReal, value: 1, changed: 'mainChanged' },
    }

    mainChanged(){
        this.getNotify()()
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true

        if(this.$properties.width) this.$properties.width.update()
        if(this.$properties.color) this.$properties.color.update()
    }
}

class QPoint extends ComplexObject {
    static defaultProperties = {
        x: { type: QReal, value: 0, changed: 'mainChanged' },
        y: { type: QReal, value: 0, changed: 'mainChanged' },
    }

    mainChanged(){
        this.getNotify()()
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true

        if(this.$properties.x) this.$properties.x.update()
        if(this.$properties.y) this.$properties.y.update()
    }
}

class QKeyNavigation extends ComplexObject {
    static defaultProperties = {
        backtab: { type: QVar, changed: 'mainChanged' },
        down: { type: QVar, changed: 'mainChanged' },
        left: { type: QVar, changed: 'mainChanged' },
        priority : { type: QString, value: '', changed: 'mainChanged' },
        right: { type: QVar, changed: 'mainChanged' },
        tab: { type: QVar, changed: 'mainChanged' },
        up: { type: QVar, changed: 'mainChanged' },
    }

    mainChanged(){
        this.getNotify()()
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true

        if(this.$properties.backtab) this.$properties.backtab.update()
        if(this.$properties.down) this.$properties.down.update()
        if(this.$properties.left) this.$properties.left.update()
        if(this.$properties.priority) this.$properties.priority.update()
        if(this.$properties.right) this.$properties.right.update()
        if(this.$properties.tab) this.$properties.tab.update()
        if(this.$properties.up) this.$properties.up.update()
    }
}

class QGeometry extends QReal {
    reset(newValue){
        if(this.frozen) return
        super.reset(newValue)
    }
    freeze(){
        this.frozen = true
    }
    unfreeze(){
        delete this.frozen
    }
}

class QAutoGeometry extends QGeometry {
    auto = true

    set(newValue){
        this.auto = false
        super.set(newValue)
    }

    setAuto(newValue){
        if(this.auto) super.set(newValue)
    }
}

class QSourceSize extends ComplexObject {
    static defaultProperties = {
        width: { type: QAutoGeometry, value: 0, changed: 'mainChanged' },
        height: { type: QAutoGeometry, value: 0, changed: 'mainChanged' },
    }

    mainChanged(){
        this.getNotify()()
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true

        if(this.$properties.width) this.$properties.width.update()
        if(this.$properties.height) this.$properties.height.update()
    }
}

class MapGestureArea extends ComplexObject {
    static NoGesture = 0
    static PinchGesture	= 1
    static PanGesture = 2
    static FlickGesture	= 3
    static RotationGesture = 4
    static TiltGesture = 5

    static defaultProperties = {
        acceptedGestures: { type: QReal, value: MapGestureArea.NoGesture, changed: 'mainChanged' },
        enabled: { type: QBool, value: false, changed: 'mainChanged' },
        flickDeceleration: { type: QReal, value: 2500, changed: 'mainChanged' },
        maximumZoomLevelChange: { type: QReal, value: 4, changed: 'mainChanged' },
        panActive: { type: QBool, value: false, changed: 'mainChanged' },
        pinchActive: { type: QBool, value: false, changed: 'mainChanged' },
        preventStealing: { type: QBool, value: false, changed: 'mainChanged' },
        rotationActive: { type: QBool, value: false, changed: 'mainChanged' },
        tiltActive: { type: QBool, value: false, changed: 'mainChanged' },
    }

    mainChanged(){
        this.getNotify()()
    }

    getNotify(){
        if(!this.notify) this.notify = new QSignal()
        return this.notify
    }

    update(){
        this.completed = true

        if(this.$properties.acceptedGestures) this.$properties.acceptedGestures.update()
        if(this.$properties.enabled) this.$properties.enabled.update()
        if(this.$properties.flickDeceleration) this.$properties.flickDeceleration.update()
        if(this.$properties.maximumZoomLevelChange) this.$properties.maximumZoomLevelChange.update()
        if(this.$properties.panActive) this.$properties.panActive.update()
        if(this.$properties.pinchActive) this.$properties.pinchActive.update()
        if(this.$properties.preventStealing) this.$properties.preventStealing.update()
        if(this.$properties.rotationActive) this.$properties.rotationActive.update()
        if(this.$properties.tiltActive) this.$properties.tiltActive.update()
    }
}

class QVariant extends QVar {
    // typeCasting(value){
    //     if(value === undefined || value === null || value instanceof this.type) {
    //         return value
    //     } else {
    //         throw `error class`
    //     }
        
    // }
}

class QModelData {
    constructor(data, index){
        this.index = index

        Object.assign(this, typeof data === 'object' ? data : {})

        return new Proxy(this, {
            has(target, name){
                return true
            },
            get(target, name){
                let caller = global.queueLink[global.queueLink.length-1]
                if(caller) {
                    if(!(target[name] instanceof QVar)) target[name] = new QVar(target[name])
                    caller.subscribe(target[name])
                }
                return target[name] instanceof QVar ? target[name].get() : target[name]
            },
            set(target, name, value){
                if(target[name] instanceof QVar){
                    target[name].reset(value)
                } else {
                    target[name] = value
                }
                
                return true
            }
        })

    }
}

module.exports.QGeometry = QGeometry
module.exports.QAutoGeometry = QAutoGeometry
module.exports.QAlias = QAlias
module.exports.QAnchorLine = QAnchorLine
module.exports.QAnchors = QAnchors
module.exports.QColor = QColor
module.exports.QInt = QInt
module.exports.QProperty = QProperty
module.exports.QReal = QReal
module.exports.QPositiveReal = QPositiveReal
module.exports.QString = QString
module.exports.QVar = QVar
module.exports.QData = QData
module.exports.QBool = QBool
module.exports.QVisible = QVisible
module.exports.QFont = QFont
module.exports.QBorder = QBorder
module.exports.QLine = QLine
module.exports.QPoint = QPoint
module.exports.QSourceSize = QSourceSize
module.exports.QKeyNavigation = QKeyNavigation
module.exports.MapGestureArea = MapGestureArea


module.exports.QVariant = QVariant
module.exports.QModelData = QModelData

