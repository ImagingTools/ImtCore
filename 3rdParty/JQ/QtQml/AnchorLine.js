class AnchorLine {
    static Left = 0
    static Right = 1
    static Top = 2
    static Bottom = 3
    static HorizontalCenter = 4
    static VerticalCenter = 5

    static create(parent, meta){
        let self = new this(parent, meta.value)
        return self
    }

    constructor(parent, float){
        this.__parent = parent
        this.__float = float
    }

    get(){
        return this
    }

    destroy(){
        delete this.__parent
        delete this.__float
    }
}

module.exports = AnchorLine