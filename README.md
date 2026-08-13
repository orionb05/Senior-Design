# Senior Design

## Trello → Discord daily digest

`.github/workflows/trello-discord-digest.yml` runs `scripts/trello_to_discord.py` once a day (12:00 UTC) and posts overdue + due-within-3-days Trello cards to a Discord channel.

### Setup (one-time)

In this repo: **Settings → Secrets and variables → Actions → New repository secret**, add:

| Secret name | Value |
|---|---|
| `TRELLO_KEY` | Your Trello API key |
| `TRELLO_TOKEN` | Your Trello API token |
| `DISCORD_WEBHOOK_URL` | The Discord channel webhook URL |

The board ID is not sensitive and is set directly in the workflow file.

### Manually trigger a run

Actions tab → "Trello Daily Discord Digest" → **Run workflow**.
