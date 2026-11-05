import unittest

from mpx_tool.currency import (
    MAX_PRICE_MINOR,
    minor_exponent,
    normalise_charge_currency,
    normalise_currency,
    parse_price_minor,
)


class CurrencyTests(unittest.TestCase):
    def test_standard_and_zero_decimal_prices(self):
        self.assertEqual(parse_price_minor("4.99", "usd"), (499, "USD"))
        self.assertEqual(parse_price_minor("100", "JPY"), (100, "JPY"))
        self.assertEqual(parse_price_minor("1.2300", "USD"), (123, "USD"))
        self.assertEqual(parse_price_minor("0", "EUR"), (0, "EUR"))
        self.assertEqual(parse_price_minor("0e-999999", "JPY"), (0, "JPY"))

    def test_rejects_excess_precision_without_context_rounding(self):
        for amount, currency in (
            ("100.1", "JPY"),
            ("1.234", "USD"),
            ("100.0000000000000000000000000001", "JPY"),
            ("1e-999999", "USD"),
        ):
            with self.subTest(amount=amount, currency=currency), self.assertRaises(ValueError):
                parse_price_minor(amount, currency)

    def test_rejects_invalid_amounts_and_currency(self):
        for amount in ("-1", "NaN", "Infinity", "not-a-number", "", "1e999999"):
            with self.subTest(amount=amount), self.assertRaises(ValueError):
                parse_price_minor(amount, "USD")
        for currency in ("ZZZ", "COU", "KWD", "CLF"):
            with self.subTest(currency=currency), self.assertRaises(ValueError):
                parse_price_minor("1.00", currency)
        with self.assertRaises(ValueError):
            parse_price_minor("92233720368547758.08", "USD")

    def test_currency_normalisation_and_legacy_exponents(self):
        self.assertEqual(normalise_currency(" jpy "), "JPY")
        self.assertEqual(normalise_currency("KWD"), "KWD")
        self.assertEqual(normalise_charge_currency(" jpy "), "JPY")
        self.assertIsNone(normalise_charge_currency("COU"))
        self.assertIsNone(normalise_currency("US"))
        self.assertIsNone(normalise_currency("ZZZ"))
        self.assertEqual(minor_exponent("JPY"), 0)
        self.assertEqual(minor_exponent("KWD"), 3)
        self.assertEqual(minor_exponent("UYW"), 4)
        self.assertEqual(minor_exponent("USD"), 2)
        self.assertEqual(parse_price_minor(str(MAX_PRICE_MINOR), "JPY"), (MAX_PRICE_MINOR, "JPY"))


if __name__ == "__main__":
    unittest.main()
