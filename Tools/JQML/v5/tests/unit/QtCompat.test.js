// Run: node --test tests/unit
// Behaviour of Qt 6 that the web client had otherwise (the problems of JQML v4 in ImtCore PR 1143, checked on v5):
// required properties of delegates, translations (context, disambiguation, numerus forms, %n), the Locale type, the
// fallback of a font family the system does not have, and images: a local source loads as if synchronously.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { runtimes } = require('./runtimes')
const { compile } = require('../../compiler/index')

const RU = `<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>Main</name>
    <message>
        <source>Open</source>
        <translation>Открыть</translation>
    </message>
    <message>
        <source>Open</source>
        <comment>a door</comment>
        <translation>Отворить</translation>
    </message>
    <message numerus="yes">
        <source>%n file(s)</source>
        <translation>
            <numerusform>%n файл</numerusform>
            <numerusform>%n файла</numerusform>
            <numerusform>%n файлов</numerusform>
        </translation>
    </message>
    <message>
        <source>Old &amp; gone</source>
        <translation type="vanished">Нет</translation>
    </message>
</context>
<context>
    <name>Other</name>
    <message>
        <source>Open</source>
        <translation>Раскрыть</translation>
    </message>
    <message id="save.title">
        <source>Save</source>
        <translation>Сохранить</translation>
    </message>
</context>
</TS>
`

function build(main){
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-compat-'))
    try {
        let entry = path.join(root, 'app', 'Main.qml')
        fs.mkdirSync(path.dirname(entry), { recursive: true })
        fs.writeFileSync(entry, main)
        let output = path.join(root, 'out')
        let result = compile({ mode: 'js', entry, output, name: 'index' })
        return { warnings: result.warnings, code: fs.readFileSync(path.join(output, 'index.js'), 'utf8') }
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

// the browser's image loader: an image loads when the test says so
class FakeImage {
    constructor(){ FakeImage.made.push(this) }
    set src(url){ this.url = url }
    finish(width, height){
        this.naturalWidth = width
        this.naturalHeight = height
        this.onload()
    }
}
FakeImage.made = []

function cases(name, runtime){
    const { engine, Item, types, translations, textLayout, Qt } = runtime
    const { Repeater, ListView, ListModel, Component, Image, Locale } = types

    // a delegate type with required properties, as the compiler makes one ($qmlRequired)
    function requiredDelegate(names, props){
        class Delegate extends Item {}
        runtime.jq.defineType(Delegate, { props })
        Delegate.$qmlRequired = names
        return new Component(null, (parent) => {
            let item
            engine.batch(() => {
                item = new Delegate(parent)
                item.height = 20
            })
            return item
        })
    }

    // ---- required properties ----

    test(name + ': the required properties of a Repeater delegate take modelData and index', () => {
        let root, repeater
        engine.batch(() => {
            root = new Item(null)
            repeater = new Repeater(root)
            repeater.delegate = requiredDelegate(['modelData', 'index'], { modelData: 'var', index: 'int' })
            repeater.model = [{ name: 'a' }, { name: 'b' }]
        })
        assert.equal(repeater.itemAt(1).modelData.name, 'b')
        assert.equal(repeater.itemAt(1).index, 1)
    })

    test(name + ': the required properties of a ListView delegate take the roles, follow their changes and the index', () => {
        let root, view, model
        engine.batch(() => {
            root = new Item(null)
            model = new ListModel(null)
            model.append({ title: 'x' })
            model.append({ title: 'y' })
            view = new ListView(root)
            view.width = 100
            view.height = 100
            view.delegate = requiredDelegate(['title', 'index'], { title: 'string', index: 'int' })
            view.model = model
        })
        engine.batch(() => view.forceLayout())
        let second = view.itemAtIndex(1)
        assert.equal(second.title, 'y')
        assert.equal(second.index, 1)
        engine.batch(() => model.setProperty(1, 'title', 'z'))
        assert.equal(second.title, 'z')
        engine.batch(() => model.insert(0, { title: 'w' }))
        assert.equal(second.index, 2)
    })

    // ---- Text lays out at componentComplete (QQuickTextPrivate::updateLayout) ----

    // ElementView in a page a Loader builds while its editor is still being built: a Column anchored to the sides of
    // its row, its rows as wide as the Column. QQuickAnchorsPrivate::updateOnComplete gives the Column its width in
    // QQuickItem::componentComplete, before QQuickBasePositioner::componentComplete positions the rows, so it is as
    // high as its rows at once, not after a polish in the frame
    test(name + ': anchors give a Column its width before it positions its rows, inside an enclosing creation', () => {
        const { Column } = types
        let row, column
        let page = new Component(null, (parent) => {
            row = new Item(parent)
            row.width = 700
            column = new Column(row)
            column.anchors.left = row.left
            column.anchors.right = row.right
            let mainPart = new Item(column)
            mainPart.$bind(Item.WIDTH, () => column.width, null)
            mainPart.height = 32
            return row
        })
        let editor = new Component(null, (parent) => {
            let root = new Item(parent)
            page.createObject(root)
            assert.equal(column.width, 700)
            assert.equal(column.height, 32)
            return root
        })
        editor.createObject(null)
        assert.equal(column.height, 32)
    })

    test(name + ': a Text made by a component lays out once, at componentComplete; outside a creation at once', () => {
        const { Text } = types
        let updates = 0
        let base = Text.prototype.$updateImplicitSize
        Text.prototype.$updateImplicitSize = function(){
            if(this.$f & engine.COMPLETED) updates++
            return base.apply(this, arguments)
        }
        try {
            let component = new Component(null, () => {
                let text = new Text(null)
                text.font.pixelSize = 20
                text.font.bold = true
                text.font.family = 'Ubuntu'
                text.text = 'hello'
                return text
            })
            let text
            engine.batch(() => { text = component.createObject(null) })
            assert.equal(updates, 1)
            assert.ok(text.implicitWidth > 0)
            updates = 0
            let loose
            engine.batch(() => {
                loose = new Text(null)
                loose.text = 'abc'
            })
            assert.ok(updates >= 1)
            assert.ok(loose.implicitWidth > 0)
        } finally {
            Text.prototype.$updateImplicitSize = base
        }
    })

    // ---- translations ----

    test(name + ': translations: the message of the context, with its disambiguation, then without it', () => {
        let catalog = translations.parseTs(RU)
        assert.equal(catalog.language, 'ru_RU')
        assert.equal(translations.translate(catalog, 'Main', 'Open'), 'Открыть')
        assert.equal(translations.translate(catalog, 'Main', 'Open', 'a door'), 'Отворить')
        assert.equal(translations.translate(catalog, 'Main', 'Open', 'unknown'), 'Открыть')
        assert.equal(translations.translate(catalog, 'Other', 'Open'), 'Раскрыть')
        assert.equal(translations.translate(catalog, 'Elsewhere', 'Open'), 'Open')
        // a vanished message is not translated, entities are read
        assert.equal(translations.translate(catalog, 'Main', 'Old & gone'), 'Old & gone')
        assert.equal(translations.translateId(catalog, 'save.title'), 'Сохранить')
    })

    test(name + ': translations: n picks the numerus form of the language and replaces %n, also untranslated', () => {
        let catalog = translations.parseTs(RU)
        assert.equal(translations.translate(catalog, 'Main', '%n file(s)', '', 1), '1 файл')
        assert.equal(translations.translate(catalog, 'Main', '%n file(s)', '', 3), '3 файла')
        assert.equal(translations.translate(catalog, 'Main', '%n file(s)', '', 5), '5 файлов')
        assert.equal(translations.translate(catalog, 'Main', '%n file(s)', '', 21), '21 файл')
        assert.equal(translations.translate(null, 'Main', 'Shared by me: %n item(s)', '', 7), 'Shared by me: 7 item(s)')
        assert.equal(translations.translate(null, 'Main', 'Total: %Ln', '', 1234567), 'Total: ' + (1234567).toLocaleString())
        assert.equal(translations.translate(null, 'Main', 'Keep %n'), 'Keep %n')
    })

    // ---- Locale ----

    test(name + ': Locale is a type: its formats, Qt.locale() is one', () => {
        assert.equal(Locale.LongFormat, 0)
        assert.equal(Locale.ShortFormat, 1)
        assert.equal(Locale.NarrowFormat, 2)
        let de = Qt.locale('de_DE')
        assert.ok(de instanceof Locale)
        assert.equal(de.name, 'de_DE')
        assert.equal(de.decimalPoint, ',')
    })

    // ---- fonts ----

    test(name + ': a font family falls back to the system UI font, not the browser default', () => {
        assert.equal(textLayout.familyCss('Segoe UI'), '"Segoe UI", system-ui, sans-serif')
        assert.equal(textLayout.familyCss('monospace'), 'monospace')
        assert.ok(textLayout.fontCss({ pixelSize: 12, family: 'Ubuntu', weight: 400, italic: false }).endsWith('12px "Ubuntu", system-ui, sans-serif'))
    })

    // ---- images ----

    test(name + ': a new local image source keeps the image it replaces until it is loaded; a loaded one is Ready at once', () => {
        let previous = globalThis.Image
        globalThis.Image = FakeImage
        FakeImage.made = []
        try {
            let image
            engine.batch(() => {
                image = new Image(null)
                image.source = 'icons/a.svg'
            })
            assert.equal(image.status, Image.Loading)
            engine.batch(() => FakeImage.made[0].finish(16, 16))
            assert.equal(image.status, Image.Ready)
            assert.equal(image.implicitWidth, 16)
            let statuses = []
            engine.connect(image, Image.SIGNAL_statusChanged, () => statuses.push(image.status))
            engine.batch(() => { image.source = 'icons/b.svg' })
            assert.equal(image.status, Image.Ready)
            assert.equal(image.implicitWidth, 16)
            engine.batch(() => FakeImage.made[1].finish(24, 24))
            assert.equal(image.implicitWidth, 24)
            // back to the loaded one: no loader, Ready with its size at once
            engine.batch(() => { image.source = 'icons/a.svg' })
            assert.equal(FakeImage.made.length, 2)
            assert.equal(image.implicitWidth, 16)
            assert.deepEqual(statuses, [])
            // a remote source goes through Loading
            engine.batch(() => { image.source = 'https://example.com/c.png' })
            assert.equal(image.status, Image.Loading)
        } finally {
            if(previous) globalThis.Image = previous
            else delete globalThis.Image
        }
    })
}

for(let [name, load] of runtimes) cases(name, load())

// ---- the compiler ----

test('qsTr of a QML file translates in the context of the file; Locale.ShortFormat is known', () => {
    let { warnings, code } = build(`import QtQuick 2.15
Item {
    property string a: qsTr("Open")
    property string b: qsTr("%n file(s)", "", 3)
    property string c: new Date().toLocaleString(Qt.locale(), Locale.ShortFormat)
}
`)
    assert.deepEqual(warnings, [])
    assert.match(code, /JQ\.qsTrIn\("Main"\)\("Open"\)/)
    assert.match(code, /JQ\.qsTrIn\("Main"\)\("%n file\(s\)","",3\)/)
})
