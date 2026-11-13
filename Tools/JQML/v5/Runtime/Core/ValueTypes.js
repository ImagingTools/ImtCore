// QML value types point / size / rect: plain fields, printed like Qt prints them (QPointF(1, 2))
class ValueType {}

class QPointF extends ValueType {
    constructor(x = 0, y = 0){
        super()
        this.x = x
        this.y = y
    }
    toString(){ return 'QPointF(' + this.x + ', ' + this.y + ')' }
}

class QSizeF extends ValueType {
    constructor(width = -1, height = -1){
        super()
        this.width = width
        this.height = height
    }
    toString(){ return 'QSizeF(' + this.width + ', ' + this.height + ')' }
}

class QRectF extends ValueType {
    constructor(x = 0, y = 0, width = 0, height = 0){
        super()
        this.x = x
        this.y = y
        this.width = width
        this.height = height
    }
    get left(){ return this.x }
    get top(){ return this.y }
    get right(){ return this.x + this.width }
    get bottom(){ return this.y + this.height }
    toString(){ return 'QRectF(' + this.x + ', ' + this.y + ', ' + this.width + ', ' + this.height + ')' }
}

// console.log(Qt.size(1, 2)) prints the value type, as QML does
function installConsole(){
    if(typeof console === 'undefined' || console.$jq2) return
    console.$jq2 = true
    for(let method of ['log', 'debug', 'info', 'warn', 'error']){
        let original = console[method]
        if(typeof original !== 'function') continue
        console[method] = function(...args){
            for(let i = 0; i < args.length; i++){
                if(args[i] instanceof ValueType) args[i] = String(args[i])
            }
            return original.apply(console, args)
        }
    }
}

module.exports = { ValueType, QPointF, QSizeF, QRectF, installConsole }
