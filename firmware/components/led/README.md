# LED component

This component will provide the shared LED interface for collector and hub firmware.

Planned public interface:

```c
void led_init(void);
void led_set(bool on);
void led_toggle(void);
```

The board-specific GPIO assignment should be provided through project configuration rather than hard-coded into application code.
