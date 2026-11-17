"""End-to-end checks for all dictionary backends (standard library only)."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
BINARY = ROOT / os.environ.get("CORRECTEUR_BINARY", "bin/correcteur")


class CorrecteurTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.directory = Path(self.temp.name)
        self.dictionary = self.directory / "dictionary.txt"
        self.text = self.directory / "text.txt"
        self.dictionary.write_text("bonjour\nmonde\nécole\ncœur\nBONJOUR\nchat\n", encoding="utf-8")
        self.text.write_text("BONJOUR, monde ! ÉCOLE cœur 123 chatt inconnu", encoding="utf-8")

    def run_cli(self, *args, stdin=None):
        return subprocess.run([str(BINARY), "-d", str(self.dictionary), *map(str, args)],
                              input=stdin, text=True, capture_output=True, cwd=ROOT)

    def test_backends_and_final_word(self):
        for method in ("hash", "trie", "seq"):
            with self.subTest(method=method):
                report = self.directory / f"{method}.txt"
                result = self.run_cli("-m", method, "-r", report, self.text)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(report.read_text(), "chatt\ninconnu\n")
                self.assertIn("Dictionnaire : 5 mots uniques", result.stdout)
                self.assertIn("Mots analyses : 6", result.stdout)
                self.assertIn("Mots inconnus : 2", result.stdout)

    def test_suggestions(self):
        result = self.run_cli("-s", "-", stdin="chatt")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("chatt -> chat", result.stdout)

    def test_suggestions_rank_distance_then_alphabet(self):
        self.dictionary.write_text("cat bat hat cats cut coat cart dog")
        result = self.run_cli("-s", "-", stdin="cot")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("cot -> cat, coat, cut, bat, cart", result.stdout)

    def test_errors(self):
        for args in (("-m", "absent", self.text), ("-d",), ("--absent",),
                     (self.directory / "missing",), ("-r", self.directory, self.text)):
            with self.subTest(args=args):
                self.assertNotEqual(self.run_cli(*args).returncode, 0)
        self.dictionary.write_text("")
        self.assertNotEqual(self.run_cli(self.text).returncode, 0)

    def test_long_tokens_and_empty_text(self):
        result = self.run_cli("-", stdin="x" * 1000 + " bonjour")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("Mots trop longs : 1", result.stdout)
        self.assertIn("Mots inconnus : 0", result.stdout)
        result = self.run_cli("-", stdin="")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("Mots analyses : 0", result.stdout)
        self.dictionary.write_text("x" * 1000)
        self.assertNotEqual(self.run_cli(self.text).returncode, 0)

    def test_invalid_utf8_preserves_report(self):
        report = self.directory / "report.txt"
        report.write_text("previous")
        self.text.write_bytes(b"bonjour \xff")
        result = self.run_cli("-r", report, self.text)
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(report.read_text(), "previous")

    def test_report_cannot_overwrite_inputs(self):
        for path in (self.dictionary, self.text):
            original = path.read_bytes()
            self.assertNotEqual(self.run_cli("-r", path, self.text).returncode, 0)
            self.assertEqual(path.read_bytes(), original)
        alias = self.directory / "alias"
        alias.symlink_to(self.dictionary)
        self.assertNotEqual(self.run_cli("-r", alias, self.text).returncode, 0)


if __name__ == "__main__":
    unittest.main()
