import copy
import json
from pathlib import Path
import re
import tempfile
import unittest

from generate import annotate, find_metatypes, load_modules, write_qmldir


class QmlToolingTests(unittest.TestCase):
    def setUp(self):
        self.document = {
            "classes": [{
                "qualifiedClassName": "example::Model",
                "classInfos": [
                    {"name": "QML.Element", "value": "OldName"},
                    {"name": "Description", "value": "Keep me"}
                ],
                "properties": [{"name": "state", "type": "QString"}],
                "methods": [{"name": "getData", "returnType": "QVariant"}]
            }]
        }

    def test_annotations_preserve_api_and_do_not_mutate_input(self):
        original = copy.deepcopy(self.document)
        result = annotate(self.document, {"class": "example::Model", "name": "Model"})
        self.assertEqual(self.document, original)
        self.assertEqual(result["classes"][0]["properties"], original["classes"][0]["properties"])
        self.assertEqual(result["classes"][0]["methods"], original["classes"][0]["methods"])
        self.assertEqual(result["classes"][0]["classInfos"], [
            {"name": "Description", "value": "Keep me"},
            {"name": "QML.Element", "value": "Model"}
        ])

    def test_singleton_and_uncreatable(self):
        result = annotate(self.document, {
            "class": "example::Model", "name": "Model", "singleton": True, "uncreatable": True
        })
        self.assertIn({"name": "QML.Singleton", "value": "true"}, result["classes"][0]["classInfos"])
        self.assertIn({"name": "QML.Creatable", "value": "false"}, result["classes"][0]["classInfos"])

    def test_unknown_class_fails(self):
        with self.assertRaisesRegex(ValueError, "MOC did not describe"):
            annotate(self.document, {"class": "example::Missing", "name": "Missing"})

    def test_missing_header_fails(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            manifest = root / "modules.json"
            manifest.write_text(json.dumps([{
                "uri": "example", "version": "1.0",
                "types": [{"name": "Model", "class": "example::Model", "header": "missing.h"}]
            }]), encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "Missing header"):
                load_modules(manifest, root)

    def test_duplicate_export_fails(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            manifest = root / "modules.json"
            entry = {"name": "Model", "class": "example::Model", "header": "modules.json"}
            manifest.write_text(json.dumps([{
                "uri": "example", "version": "1.0", "types": [entry, entry]
            }]), encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "Duplicate QML export"):
                load_modules(manifest, root)

    def test_mixed_module_preserves_qml_exports_and_source(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            source = root / "source"
            source.mkdir()
            original = "module example\nsingleton Style 1.0 Style.qml\ntypeinfo old.qmltypes\n"
            (source / "qmldir").write_text(original, encoding="utf-8")
            (source / "Style.qml").write_text("pragma Singleton\nimport QtQuick\nQtObject {}\n", encoding="utf-8")
            output = root / "output"
            write_qmldir({"uri": "example", "qml_directory": "source"}, root, output)
            self.assertEqual((source / "qmldir").read_text(encoding="utf-8"), original)
            self.assertEqual((output / "qmldir").read_text(encoding="utf-8"),
                             "module example\nsingleton Style 1.0 Style.qml\ntypeinfo plugins.qmltypes\n")
            self.assertTrue((output / "Style.qml").is_file())

    def test_qt_metatypes_with_and_without_configuration_suffix(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            directory = root / "metatypes"
            directory.mkdir()
            configured = directory / "qt6core_relwithdebinfo_metatypes.json"
            configured.touch()
            self.assertEqual(find_metatypes(root, "Core"), configured)
            plain = directory / "qt6core_metatypes.json"
            plain.touch()
            self.assertEqual(find_metatypes(root, "Core"), plain)
            with self.assertRaisesRegex(ValueError, "Cannot select Qt metatypes"):
                find_metatypes(root, "Core5Compat")

    def test_imtqml_manifest_matches_runtime_registrations(self):
        source_root = Path(__file__).resolve().parents[2]
        modules = load_modules(Path(__file__).with_name("modules.json"), source_root)
        module = next(module for module in modules if module["uri"] == "com.imtcore.imtqml")
        runtime = set()
        for name in ("CStaticQmlBaseTypeRegistratorComp.cpp", "CStaticQmlTypeRegistratorComp.cpp"):
            source = (source_root / "Include/imtqml" / name).read_text(encoding="utf-8")
            for match in re.finditer(
                r'qmlRegister(Type|SingletonInstance)<([^>]+)>\("([^"]+)",\s*(\d+),\s*(\d+),\s*"([^"]+)"', source
            ):
                kind, cpp_class, uri, major, minor, qml_name = match.groups()
                runtime.add((uri, major + "." + minor, cpp_class, qml_name, kind == "SingletonInstance"))
        tooling = {
            (module["uri"], module["version"], entry["class"], entry["name"], entry.get("singleton", False))
            for entry in module["types"]
        }
        self.assertEqual(tooling, runtime)


if __name__ == "__main__":
    unittest.main()
