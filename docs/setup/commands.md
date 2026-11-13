# Common development commands

These commands are run from one of the ESP-IDF application directories:

```text
firmware/collector/
firmware/hub/
```

## Team toolchain version

Use the same version across the team:

```text
ESP-IDF: v6.1
Targets: esp32, esp32s3
```

ESP-IDF itself is a machine-local toolchain, not project source. Do not commit it to this repository. The repository-root `esp/` directory is ignored for local installations; keeping the installation outside the repository, such as `~/esp/esp-idf-v6.1`, is also acceptable.

## Activate ESP-IDF

Use an ESP-IDF terminal or activate the installed environment before running `idf.py`.

Linux/macOS:

```bash
. $IDF_PATH/export.sh
```

If the local installation is currently at `esp/xit/` in this repository, activate it from the repository root with:

```bash
source esp/xit/export.sh
```

Windows:

Use the ESP-IDF PowerShell/Command Prompt or the terminal provided by the ESP-IDF VS Code extension.

## Select the chip target

Run once for the board being built:

```bash
idf.py set-target esp32
idf.py set-target esp32s3
```

## Configure and build

```bash
idf.py menuconfig
idf.py build
```

The initial collector bring-up application prints startup information and a periodic serial heartbeat. Its options are under `SCESM Collector` in `menuconfig`.

## Flash and monitor

Linux examples:

```bash
idf.py -p /dev/ttyACM0 flash
idf.py -p /dev/ttyACM0 monitor
idf.py -p /dev/ttyACM0 flash monitor
```

Windows example:

```powershell
idf.py -p COM5 flash monitor
```

Replace the port with the one assigned to the connected board.

## Clean and reconfigure

```bash
idf.py clean
idf.py fullclean
idf.py reconfigure
```

Use `fullclean` when changing targets or when the build configuration becomes stale.

## Component dependencies

```bash
idf.py create-component component_name
idf.py create-manifest
idf.py add-dependency namespace/component
idf.py update-dependencies
```

Do not manually edit generated `build/`, `managed_components/`, or `sdkconfig` files.

## Team conventions

- Build collector firmware from `firmware/collector/`.
- Build hub firmware from `firmware/hub/`.
- Record the board type and serial port before flashing.
- Confirm the target before flashing: `esp32` versus `esp32s3`.
- Keep the ESP-IDF version consistent across the team.
