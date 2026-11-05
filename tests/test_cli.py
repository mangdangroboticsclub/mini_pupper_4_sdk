import argparse
import unittest

from mpx_tool.cli import build_parser
from mpx_tool.resources import DEVELOPER_PORTAL_URL, SKILL_TESTER_URL


class CliResourceTests(unittest.TestCase):
    def test_top_level_help_mentions_developer_resources(self):
        help_text = build_parser().format_help()
        self.assertIn(DEVELOPER_PORTAL_URL, help_text)
        self.assertIn(SKILL_TESTER_URL, help_text)

    def test_publish_help_mentions_developer_resources(self):
        parser = build_parser()
        subparsers = next(
            action for action in parser._actions
            if isinstance(action, argparse._SubParsersAction)
        )
        help_text = subparsers.choices["publish"].format_help()
        self.assertIn(DEVELOPER_PORTAL_URL, help_text)
        self.assertIn(SKILL_TESTER_URL, help_text)


if __name__ == "__main__":
    unittest.main()
