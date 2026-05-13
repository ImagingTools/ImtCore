const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QInt, QVisible } = require('../utils/properties')

class Menu extends Item {
    static defaultProperties = {
        title: { type: QString, value: '', changed: '$titleChanged' },
        visible: { type: QVisible, value: false, changed: '$visibleChanged' },
        enabled: { type: QBool, value: true },
        count: { type: QInt, value: 0 },
        currentIndex: { type: QInt, value: -1 },
        contentItem: { type: QVar },
        cascade: { type: QBool, value: true },
        overlap: { type: QReal, value: 0 },
        delegate: { type: QVar },
        focus: { type: QBool, value: false },
    }

    static defaultSignals = {
        aboutToShow: { params: [] },
        aboutToHide: { params: [] },
        opened: { params: [] },
        closed: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.setStyle({
            position: 'fixed',
            display: 'none',
            flexDirection: 'column',
            backgroundColor: '#ffffff',
            border: '1px solid #cccccc',
            borderRadius: '4px',
            boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
            padding: '4px 0',
            zIndex: '10000',
            minWidth: '120px',
            overflow: 'visible',
        })

        this.$onDocClick = (e) => {
            if (this.getPropertyValue('visible') && !this.getDom().contains(e.target)){
                this.close()
            }
        }
    }

    addChild(child){
        super.addChild(child)
        if (child.getProperty && child.getProperty('menu')){
            child.getProperty('menu').reset(this)
        }
        this.getProperty('count').reset(this.getProperty('children').get().length)
    }

    open(){
        if (this.$signals.aboutToShow) this.$signals.aboutToShow()
        this.getProperty('visible').reset(true)
    }

    close(){
        if (this.$signals.aboutToHide) this.$signals.aboutToHide()
        this.getProperty('visible').reset(false)
    }

    popup(x, y){
        if (x !== undefined && y !== undefined){
            this.setStyle({
                left: `${x}px`,
                top: `${y}px`,
            })
        }
        this.open()
    }

    dismiss(){
        this.close()
    }

    itemAt(index){
        let children = this.getProperty('children').get()
        return (index >= 0 && index < children.length) ? children[index] : null
    }

    $visibleChanged(){
        if (this.getPropertyValue('visible')){
            this.setStyle({ display: 'flex' })
            setTimeout(() => {
                document.addEventListener('click', this.$onDocClick, true)
            }, 0)
            if (this.$signals.opened) this.$signals.opened()
        } else {
            this.setStyle({ display: 'none' })
            document.removeEventListener('click', this.$onDocClick, true)
            if (this.$signals.closed) this.$signals.closed()
        }
    }

    $titleChanged(){
        // title is used by MenuBar to display the menu trigger label
    }

    destroy(){
        document.removeEventListener('click', this.$onDocClick, true)
        delete this.$onDocClick
        super.destroy()
    }
}

module.exports.Menu = Menu
