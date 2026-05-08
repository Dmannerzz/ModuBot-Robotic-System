# Arduino Firmware

Arduino-based firmware implementation for the ModuBot robotic platform.

This version contains the original embedded control architecture used during the early development of ModuBot, including:

- Manual remote-controlled navigation
- Obstacle avoidance behavior
- Patrol mode systems
- Route logging and playback
- Ultrasonic sensor integration
- Motor control systems
- FSM-based robot behaviors

The Arduino implementation served as the foundation for ModuBot’s robotics architecture and subsystem experimentation.

---

## Development Status

The planned refactor of the Arduino firmware into a cleaner FSM-based modular architecture was discontinued after the project was successfully migrated to the ESP32 platform.

Since ongoing subsystem testing, architectural improvements, and robotics experimentation are now being performed on ESP32 hardware, development efforts were consolidated there rather than maintaining two parallel firmware architectures.

As a result, the Arduino implementation is now preserved primarily for:

- Historical development tracking
- Architecture reference
- Embedded systems experimentation
- Legacy firmware support

---

## Full Firmware Archive

The complete Arduino firmware codebase has been preserved in:

```txt id="r85g0w"
ModuBot_ArduinoOS
```

This archive contains the final consolidated Arduino implementation before active development focus shifted fully to the ESP32 platform.

---

## Notes

The Arduino firmware reflects an earlier stage of the ModuBot architecture and may not contain the latest subsystem refactors, FSM redesigns, or behavioral improvements currently being developed on the ESP32 implementation.
