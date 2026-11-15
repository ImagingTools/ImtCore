const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { resolveUrl } = require('../Core/Resources')
const { defineType } = require('../Core/type')

// ---- FontLoader ---------------------------------------------------------------------------------

// status: Null 0, Ready 1, Loading 2, Error 3; name is the family to use in font.family
class FontLoader extends QtObject {
    $load(){
        let source = this.$v[FontLoader.SOURCE]
        if(this.$face && typeof document !== 'undefined') document.fonts.delete(this.$face)
        this.$face = null
        if(!source || typeof FontFace === 'undefined'){
            this.$write(FontLoader.STATUS, 0)
            return
        }
        let name = String(source).split('/').pop().split('?')[0].replace(/\.\w+$/, '')
        let face = new FontFace(name, 'url("' + resolveUrl(source) + '")')
        this.$face = face
        this.$write(FontLoader.STATUS, 2)
        face.load().then(() => {
            if(this.$face !== face || this.$f & engine.DESTROYED) return
            document.fonts.add(face)
            engine.batch(() => {
                this.$write(FontLoader.NAME, name)
                this.$write(FontLoader.STATUS, 1)
            })
        }, () => {
            if(this.$face !== face || this.$f & engine.DESTROYED) return
            engine.batch(() => this.$write(FontLoader.STATUS, 3))
        })
    }

    get font(){ return { family: this.$v[FontLoader.NAME] } }
}
defineType(FontLoader, {
    props: {
        source: { kind: 'string', effect: (loader) => loader.$load() },
        name: { kind: 'string', readonly: true },
        status: { kind: 'int', readonly: true },
    },
})
Object.assign(FontLoader, { Null: 0, Ready: 1, Loading: 2, Error: 3 })

module.exports = { FontLoader }
