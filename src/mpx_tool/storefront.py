"""Validation for storefront metadata stored in ``manifest.json``."""

from __future__ import annotations

from urllib.parse import urlsplit

MAX_TAGS = 16
MAX_TAG_LENGTH = 40
MAX_ICON_URL_LENGTH = 2048


def normalise_tags(value: object) -> list[str]:
    """Trim and case-insensitively deduplicate a manifest tag array."""
    if value is None:
        return []
    if not isinstance(value, list):
        raise ValueError("Manifest 'tags' must be an array of strings.")
    if len(value) > MAX_TAGS:
        raise ValueError(f"Manifest 'tags' supports at most {MAX_TAGS} entries.")

    tags: list[str] = []
    seen: set[str] = set()
    for raw in value:
        if not isinstance(raw, str):
            raise ValueError("Manifest 'tags' must contain only strings.")
        tag = raw.strip()
        if not tag:
            continue
        if len(tag) > MAX_TAG_LENGTH:
            raise ValueError(f"Manifest tag {tag!r} exceeds {MAX_TAG_LENGTH} characters.")
        folded = tag.casefold()
        if folded not in seen:
            seen.add(folded)
            tags.append(tag)
    return tags


def normalise_icon_url(value: object) -> str | None:
    """Return a validated HTTPS icon URL; an omitted/blank value means none."""
    if value is None or value == "":
        return None
    if not isinstance(value, str):
        raise ValueError("Manifest 'icon_url' must be an HTTPS URL string.")
    url = value.strip()
    if not url:
        return None
    if len(url) > MAX_ICON_URL_LENGTH:
        raise ValueError(f"Manifest 'icon_url' exceeds {MAX_ICON_URL_LENGTH} characters.")
    parsed = urlsplit(url)
    if parsed.scheme.lower() != "https" or not parsed.netloc:
        raise ValueError("Manifest 'icon_url' must be an absolute HTTPS URL.")
    if parsed.username is not None or parsed.password is not None:
        raise ValueError("Manifest 'icon_url' must not contain credentials.")
    return url


def normalise_manifest_metadata(manifest: dict) -> dict:
    """Copy a manifest, normalizing only metadata fields actually supplied.

    Omission is significant during republish: the gateway preserves existing
    listing metadata for omitted fields, while explicit empty values clear it.
    """
    result = manifest.copy()
    if "tags" in result:
        result["tags"] = normalise_tags(result["tags"])
    if "icon_url" in result:
        result["icon_url"] = normalise_icon_url(result["icon_url"]) or ""
    return result
