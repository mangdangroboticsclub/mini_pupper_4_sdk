"""Currency validation and decimal-to-minor-unit conversion.

The marketplace stores prices in the currency's actual minor unit (cents for
USD, whole yen for JPY). Keep this module dependency-free so validation also
works in minimal CLI installations.
"""

from __future__ import annotations

from decimal import Decimal, DecimalException, InvalidOperation, localcontext

# Active ISO 4217 codes for legacy data and FX display. Chargeable listings
# use the narrower provider-supported CHARGE_CURRENCIES set below.
SUPPORTED_CURRENCIES = frozenset("""
AED AFN ALL AMD ANG AOA ARS AUD AWG AZN
BAM BBD BDT BGN BHD BIF BMD BND BOB BOV BRL BSD BTN BWP BYN BZD
CAD CDF CHE CHF CHW CLF CLP CNY COP COU CRC CUP CVE CZK
DJF DKK DOP DZD
EGP ERN ETB EUR
FJD FKP
GBP GEL GHS GIP GMD GNF GTQ GYD
HKD HNL HTG HUF
IDR ILS INR IQD IRR ISK
JMD JOD JPY
KES KGS KHR KMF KPW KRW KWD KYD KZT
LAK LBP LKR LRD LSL LYD
MAD MDL MGA MKD MMK MNT MOP MRU MUR MVR MWK MXN MXV MYR MZN
NAD NGN NIO NOK NPR NZD
OMR
PAB PEN PGK PHP PKR PLN PYG
QAR
RON RSD RUB RWF
SAR SBD SCR SDG SEK SGD SHP SLE SOS SRD SSP STN SVC SYP SZL
THB TJS TMT TND TOP TRY TTD TWD TZS
UAH UGX USD USN UYI UYU UYW UZS
VED VES VND VUV
WST
XAF XCD XCG XOF XPF
YER
ZAR ZMW ZWG
""".split())

# Stripe's supported presentment currencies (https://docs.stripe.com/currencies).
# Keep ISO funds/metal/index codes out of listings: they cannot be charged.
CHARGE_CURRENCIES = frozenset("""
USD AED AFN ALL AMD ANG AOA ARS AUD AWG AZN
BAM BBD BDT BIF BMD BND BOB BRL BSD BWP BYN BZD
CAD CDF CHF CLP CNY COP CRC CVE CZK
DJF DKK DOP DZD EGP ETB EUR
FJD FKP GBP GEL GIP GMD GNF GTQ GYD
HKD HNL HTG HUF IDR ILS INR ISK
JMD JPY KES KGS KHR KMF KRW KYD KZT
LAK LBP LKR LRD LSL MAD MDL MGA MKD MMK MNT MOP MUR MVR MWK MXN MYR MZN
NAD NGN NIO NOK NPR NZD PAB PEN PGK PHP PKR PLN PYG
QAR RON RSD RUB RWF SAR SBD SCR SEK SGD SHP SLE SOS SRD STD SZL
THB TJS TOP TRY TTD TWD TZS UAH UGX UYU UZS
VND VUV WST XAF XCD XCG XOF XPF YER ZAR ZMW
""".split())

# Minor-unit behavior matches the marketplace payment provider. Some of these
# differ from ISO's nominal exponent (for example IDR and VND are charged as
# zero-decimal currencies).
ZERO_DECIMAL = frozenset("""
BIF CLP DJF GNF IDR ISK JPY KMF KRW LAK MGA PYG RWF UGX UYI VND VUV XAF XOF XPF
""".split())
THREE_DECIMAL = frozenset("BHD IQD JOD KWD LYD OMR TND".split())
FOUR_DECIMAL = frozenset({"CLF", "UYW"})

MAX_PRICE_MINOR = 9_223_372_036_854_775_807


def normalise_currency(code: str | None) -> str | None:
    """Return an uppercase supported currency code, or ``None``."""
    if not isinstance(code, str):
        return None
    currency = code.strip().upper()
    return currency if currency in SUPPORTED_CURRENCIES else None


def normalise_charge_currency(code: str | None) -> str | None:
    """Return an uppercase currency supported for marketplace charges."""
    if not isinstance(code, str):
        return None
    currency = code.strip().upper()
    return currency if currency in CHARGE_CURRENCIES else None


def minor_exponent(currency: str) -> int:
    """Return the number of decimal digits used when charging ``currency``."""
    code = (currency or "").strip().upper()
    if code in ZERO_DECIMAL:
        return 0
    if code in THREE_DECIMAL:
        return 3
    if code in FOUR_DECIMAL:
        return 4
    return 2


def parse_price_minor(value: str, currency: str) -> tuple[int, str]:
    """Validate a major-unit price and return ``(minor_units, currency)``.

    Examples: ``4.99 USD`` becomes ``(499, 'USD')``; ``100 JPY`` becomes
    ``(100, 'JPY')``. Raises ``ValueError`` with a CLI-friendly explanation.
    """
    code = normalise_charge_currency(currency)
    if code is None:
        raise ValueError(
            f"Unsupported charge currency {currency!r}. Use a provider-supported ISO code such as USD, EUR or JPY."
        )

    try:
        amount = Decimal(str(value).strip())
    except (InvalidOperation, ValueError):
        raise ValueError(f"Invalid price {value!r}. Use a non-negative number such as 4.99.") from None

    if not amount.is_finite() or amount < 0:
        raise ValueError("Price must be a finite, non-negative number.")

    if amount.is_zero():
        return 0, code

    exponent = minor_exponent(code)
    maximum = Decimal(MAX_PRICE_MINOR).scaleb(-exponent)
    if amount > maximum:
        raise ValueError("Price is too large for the marketplace.")

    # Inspect the coefficient before scaling. Decimal.scaleb() obeys context
    # precision and can otherwise silently round an over-precise price into an
    # integer (for example 100.0000000000000000000000000001 JPY).
    decimal_exponent = amount.as_tuple().exponent
    if decimal_exponent < -exponent:
        excess = -exponent - decimal_exponent
        if any(amount.as_tuple().digits[-excess:]):
            noun = "decimal place" if exponent == 1 else "decimal places"
            raise ValueError(f"{code} prices support {exponent} {noun}; got {value!r}.")

    try:
        with localcontext() as context:
            context.prec = max(28, len(amount.as_tuple().digits) + exponent + 2)
            minor = int(amount.scaleb(exponent))
    except (DecimalException, OverflowError, ValueError):
        raise ValueError(f"Invalid price {value!r} for {code}.") from None
    return minor, code
