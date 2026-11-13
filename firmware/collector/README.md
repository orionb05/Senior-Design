# Collector firmware

This is the ESP-IDF project for a SCESM collector node.

Initial bring-up:

```bash
cd firmware/collector
idf.py set-target esp32s3
idf.py build
idf.py -p <serial-port> flash monitor
```

The shared bring-up component prints startup information and a periodic serial heartbeat. Configure it with:

```bash
idf.py menuconfig
```

Then open `SCESM Collector` to enable or disable the bring-up heartbeat and change its interval.
