"""Change visibility for a skill owned by the logged-in developer."""

from __future__ import annotations

import argparse
import sys

from mpx_tool.sdk.auth import get_username_from_token, read_token
from mpx_tool.sdk.gateway import GatewayClient, GatewayError


def add_visibility_parser(sub: argparse._SubParsersAction) -> None:
    parser = sub.add_parser("visibility", help="Show or hide a published skill")
    parser.add_argument("skill", help="Full skill ID, or slug owned by the logged-in user")
    parser.add_argument("visibility", choices=("public", "hidden"))


def cmd_visibility(args: argparse.Namespace) -> None:
    token = read_token()
    if not token:
        print("❌ Not logged in. Run 'mpx login' first.", file=sys.stderr)
        sys.exit(1)
    username = get_username_from_token(token)
    if not username:
        print("❌ Invalid token. Run 'mpx login' again.", file=sys.stderr)
        sys.exit(1)

    skill_id = args.skill if "~" in args.skill else f"{username}~{args.skill}"
    client = GatewayClient(gateway_url=getattr(args, "gateway_url", None))
    try:
        status, body = client.set_visibility(skill_id, args.visibility, token)
    except GatewayError as exc:
        print(f"❌ {exc}", file=sys.stderr)
        sys.exit(1)

    if status != 200:
        message = body.get("error", str(body)) if isinstance(body, dict) else str(body)
        print(f"❌ Visibility update failed (HTTP {status}): {message}", file=sys.stderr)
        sys.exit(1)

    label = "public" if args.visibility == "public" else "hidden"
    print(f"✅ '{skill_id}' is now {label}.")
