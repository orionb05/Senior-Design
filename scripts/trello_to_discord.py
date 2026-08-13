#!/usr/bin/env python3
"""Daily digest: post overdue/due-soon Trello cards to a Discord channel via webhook.

Reads credentials from environment variables (set as GitHub Actions secrets):
  TRELLO_KEY, TRELLO_TOKEN, TRELLO_BOARD_ID, DISCORD_WEBHOOK_URL

Uses only the Python standard library so no dependency install step is needed in CI.
"""
import json
import os
import sys
import urllib.parse
import urllib.request
from datetime import datetime, timezone, timedelta

TRELLO_KEY = os.environ["TRELLO_KEY"]
TRELLO_TOKEN = os.environ["TRELLO_TOKEN"]
TRELLO_BOARD_ID = os.environ["TRELLO_BOARD_ID"]
DISCORD_WEBHOOK_URL = os.environ["DISCORD_WEBHOOK_URL"]

DUE_SOON_WINDOW_DAYS = 3

SEVERITY_COLORS = {
    "overdue": 0xE53935,   # red
    "due_soon": 0xFBC02D,  # yellow
}


def trello_get(path, **params):
    params.update({"key": TRELLO_KEY, "token": TRELLO_TOKEN})
    url = f"https://api.trello.com/1{path}?{urllib.parse.urlencode(params)}"
    with urllib.request.urlopen(url, timeout=20) as resp:
        return json.loads(resp.read().decode())


def fetch_cards_and_lists():
    cards = trello_get(
        f"/boards/{TRELLO_BOARD_ID}/cards",
        fields="name,due,dueComplete,idList,url,labels",
    )
    lists = trello_get(f"/boards/{TRELLO_BOARD_ID}/lists", fields="name")
    list_names = {lst["id"]: lst["name"] for lst in lists}
    return cards, list_names


def categorize(cards, list_names):
    now = datetime.now(timezone.utc)
    soon_cutoff = now + timedelta(days=DUE_SOON_WINDOW_DAYS)
    overdue, due_soon = [], []

    for card in cards:
        if card.get("dueComplete") or not card.get("due"):
            continue
        due = datetime.fromisoformat(card["due"].replace("Z", "+00:00"))
        entry = {
            "name": card["name"],
            "due": due,
            "list": list_names.get(card["idList"], "Unknown"),
            "url": card["url"],
        }
        if due < now:
            overdue.append(entry)
        elif due <= soon_cutoff:
            due_soon.append(entry)

    overdue.sort(key=lambda c: c["due"])
    due_soon.sort(key=lambda c: c["due"])
    return overdue, due_soon


def format_field(entries):
    lines = []
    for e in entries:
        due_str = e["due"].strftime("%a %b %d")
        lines.append(f"**[{e['name']}]({e['url']})** — {e['list']} — due {due_str}")
    return "\n".join(lines) if lines else "None"


def build_embeds(overdue, due_soon):
    if not overdue and not due_soon:
        return [{
            "title": "📋 Senior Design Timeline — Daily Digest",
            "description": "Nothing overdue or due in the next "
                            f"{DUE_SOON_WINDOW_DAYS} days. Nothing urgent right now.",
            "color": 0x43A047,
        }]

    embeds = [{
        "title": "📋 Senior Design Timeline — Daily Digest",
        "color": SEVERITY_COLORS["overdue"] if overdue else SEVERITY_COLORS["due_soon"],
        "fields": [],
    }]
    if overdue:
        embeds[0]["fields"].append({
            "name": f"🔴 Overdue ({len(overdue)})",
            "value": format_field(overdue),
            "inline": False,
        })
    if due_soon:
        embeds[0]["fields"].append({
            "name": f"🟡 Due within {DUE_SOON_WINDOW_DAYS} days ({len(due_soon)})",
            "value": format_field(due_soon),
            "inline": False,
        })
    return embeds


def post_to_discord(embeds):
    body = json.dumps({"embeds": embeds}).encode()
    req = urllib.request.Request(
        DISCORD_WEBHOOK_URL,
        data=body,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    with urllib.request.urlopen(req, timeout=20) as resp:
        if resp.status not in (200, 204):
            raise RuntimeError(f"Discord webhook returned status {resp.status}")


def main():
    cards, list_names = fetch_cards_and_lists()
    overdue, due_soon = categorize(cards, list_names)
    embeds = build_embeds(overdue, due_soon)
    post_to_discord(embeds)
    print(f"Posted digest: {len(overdue)} overdue, {len(due_soon)} due soon.")


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:  # noqa: BLE001
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)
