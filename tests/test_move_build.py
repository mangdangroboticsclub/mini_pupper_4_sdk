import io
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

from mpx_tool.commands.move.build import cmd_move_build
from mpx_tool.resources import SKILL_TESTER_URL


class MoveBuildResourceTests(unittest.TestCase):
    def test_successful_build_shows_skill_tester(self):
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / "src" / "dance.wat"
            source.parent.mkdir()
            source.write_text("(module)")
            args = SimpleNamespace(
                show_toolchains=False,
                source=str(source),
                output=None,
                local=True,
                validate=False,
                inspect=False,
            )

            def fake_compile(source, output, cwd):
                output.parent.mkdir(parents=True, exist_ok=True)
                output.write_bytes(b"\0asm")

            stdout = io.StringIO()
            with patch("mpx_tool.commands.move.build._compile_local", fake_compile):
                with redirect_stdout(stdout):
                    cmd_move_build(args)

            self.assertIn("✅ Compiled", stdout.getvalue())
            self.assertIn(SKILL_TESTER_URL, stdout.getvalue())


if __name__ == "__main__":
    unittest.main()
