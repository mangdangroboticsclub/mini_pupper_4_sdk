import unittest

from mpx_tool.storefront import normalise_icon_url, normalise_manifest_metadata, normalise_tags


class StorefrontMetadataTests(unittest.TestCase):
    def test_tags_are_trimmed_and_case_insensitively_deduplicated(self):
        self.assertEqual(normalise_tags([" Dance ", "dance", "Moves", ""]), ["Dance", "Moves"])
        self.assertEqual(normalise_tags(None), [])

    def test_tags_must_be_a_bounded_string_array(self):
        for value in ("dance", [1], ["x" * 41], [str(i) for i in range(17)]):
            with self.subTest(value=value), self.assertRaises(ValueError):
                normalise_tags(value)

    def test_manifest_normalisation_preserves_omission_semantics(self):
        unchanged = normalise_manifest_metadata({"title": "Legacy"})
        self.assertNotIn("tags", unchanged)
        self.assertNotIn("icon_url", unchanged)
        cleared = normalise_manifest_metadata({"tags": [], "icon_url": ""})
        self.assertEqual(cleared, {"tags": [], "icon_url": ""})
        normalised = normalise_manifest_metadata({"tags": [" Dance ", "dance"]})
        self.assertEqual(normalised["tags"], ["Dance"])
        self.assertNotIn("icon_url", normalised)

    def test_icon_requires_an_absolute_https_url(self):
        url = "https://cdn.example.com/icons/dance.png"
        self.assertEqual(normalise_icon_url(url), url)
        self.assertIsNone(normalise_icon_url(""))
        self.assertIsNone(normalise_icon_url(None))
        for value in ("http://example.com/icon.png", "/icon.png", "https://user:pass@example.com/i.png", 42):
            with self.subTest(value=value), self.assertRaises(ValueError):
                normalise_icon_url(value)


if __name__ == "__main__":
    unittest.main()
