// Standalone test for stable, deterministic DOM ids (`data-qml-id`).
//
// The web compiler generates QML code dynamically. Previously every generation
// produced different DOM identifiers, which made visual / E2E testing unreliable.
// The compiler now emits a deterministic per-object local key (via
// `__applyQmlId(...)`) that is combined at runtime into a stable path-based
// `data-qml-id`.
//
// This test compiles a fixture twice and asserts that:
//   1. the generated code is byte-for-byte identical across regenerations, and
//   2. the deterministic set of emitted local keys matches an expected snapshot.
//
// Both properties are what guarantee that DOM ids stay stable between builds.
//
// Run with: node tests/stableId.test.js

const fs = require('fs')
const os = require('os')
const path = require('path')

const compiler = require('../compiler/compiler')

const fixtureDir = path.resolve(__dirname, 'WebStableId')

function compileFixture(outputDir) {
    fs.mkdirSync(outputDir, { recursive: true })

    compiler.compile({
        config: path.resolve(fixtureDir, 'test.json'),
        output: outputDir,
        entry: path.resolve(fixtureDir, 'Main.qml'),
        name: 'test',
        root: '/',
        mode: 'js',
    })

    return fs.readFileSync(path.resolve(outputDir, 'test.js'), 'utf8')
}

// Drop the trailing source-map comment, which is the only line that legitimately
// depends on the absolute output path and is irrelevant to id stability.
function stripSourceMap(code) {
    return code.replace(/\n?\/\/# sourceMappingURL=.*$/m, '')
}

function extractLocalKeys(code) {
    const keys = []
    const re = /__applyQmlId\(\s*`([^`]*)`/g
    let m
    while ((m = re.exec(code)) !== null) {
        keys.push(m[1])
    }
    return keys
}

function main() {
    const dirA = fs.mkdtempSync(path.join(os.tmpdir(), 'qmlid-a-'))
    const dirB = fs.mkdtempSync(path.join(os.tmpdir(), 'qmlid-b-'))

    let failures = 0
    const fail = (msg) => {
        failures++
        console.error(`[-] ${msg}`)
    }

    const codeA = stripSourceMap(compileFixture(dirA))
    const codeB = stripSourceMap(compileFixture(dirB))

    // 1. Deterministic generation: two independent compilations are identical.
    if (codeA !== codeB) {
        fail('Generated code is not byte-for-byte identical across regenerations.')
    } else {
        console.log('[+] Generated code is identical across regenerations.')
    }

    const keys = extractLocalKeys(codeA)

    // 2. Every object emits a deterministic local key.
    if (keys.length === 0) {
        fail('No __applyQmlId(...) local keys were emitted.')
    }

    // The local keys are derived from author-provided `id:` values, so they are
    // human-readable and must stay stable. Reused components (Box) and the
    // Repeater delegate all share the same internal `boxRoot`/`caption` keys;
    // uniqueness is achieved at runtime via the parent path + model index.
    const expected = ['root', 'header', 'boxRoot', 'caption', 'firstBox', 'secondBox', 'rep']
    for (const e of expected) {
        if (!keys.includes(e)) {
            fail(`Expected local key "${e}" was not emitted. Got: ${JSON.stringify(keys)}`)
        }
    }
    if (failures === 0) {
        console.log(`[+] Emitted deterministic local keys: ${JSON.stringify(keys)}`)
    }

    fs.rmSync(dirA, { recursive: true, force: true })
    fs.rmSync(dirB, { recursive: true, force: true })

    if (failures > 0) {
        console.error(`[i] stableId test FAILED with ${failures} error(s).`)
        process.exitCode = 1
    } else {
        console.log('[i] stableId test PASSED.')
    }
}

main()
