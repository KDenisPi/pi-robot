# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

pi-robot is a C++17 framework for building Raspberry Pi robotics applications. It targets both x86_64 (development/emulation) and armv7l (Raspberry Pi hardware). The `weather` application in `weather/` is a complete reference implementation using the framework.

## Build

Third-party dependencies live in `~/third-party/` (dev machine) or `../third-party/` (when `REAL_HARDWARE` is set). Run `./third-party.sh` once to set them up.

System packages required: `libspdlog-dev`, `libmosquitto-dev`, `libfmt-dev` (x86_64 only), optionally `libcurl4-openssl-dev`.

```bash
# Configure (from repo root)
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# For real Raspberry Pi hardware
cmake .. -DREAL_HARDWARE=ON

# Optional feature flags
cmake .. -DUSE_SQL_STORAGE=ON

# Build everything
make -j$(nproc)

# Build weather application separately (not included in main build)
cd weather && mkdir -p build && cd build && cmake .. && make -j$(nproc)
```

## Running Tests

Tests require `sudo` because they access hardware/system resources:

```bash
cd build
ctest                          # run all tests
sudo ./test_cbuff               # circular buffer test
sudo ./test_fstor               # file storage test
```

## Running an Application

Applications require a JSON configuration file:

```bash
# Non-daemon mode (foreground, useful for development)
./weather --conf /path/to/config.json --nodaemon --llevel DEBUG

# With MQTT
./weather --conf /path/to/config.json --mqtt-conf /path/to/mqtt.json --nodaemon

# Override first state
./weather --conf /path/to/config.json --fstate StInitialization

# Signal controls (once running)
kill -USR1 <pid>   # start state machine
kill -USR2 <pid>   # stop state machine
kill -HUP  <pid>   # reload log level from /var/log/pi-robot/logger.conf
```

Logs go to `/var/log/pi-robot/`, data files to `/var/data/pi-robot/data`.

## Architecture

### Module Dependency Order

```
pi-utils  ←  pi-core
          ←  pi-mqtt
          ←  pi-env  (depends on pi-mqtt)
          ←  pi-http
          ←  pi-library  (depends on pi-utils, pi-core)
          ←  pi-smachine (depends on pi-library, pi-mqtt)
          ←  pi-main     (header-only template, glues all)
          ←  <app>       (weather, etc.)
```

### Core Modules

**`pi-utils/`** — Foundation utilities used by all modules:
- `logger.h` / `logger.cpp`: Async logger via spdlog with a circular buffer. Log levels: `ERROR=0`, `NECECCARY=1`, `INFO=2`, `DEBUG=3`. Call `logger::log(level, tag, message)`.
- `Threaded.h`: CRTP base for any class that runs on its own thread. Pattern: `start<DerivedClass>(this)` + static `worker(DerivedClass*)`.
- `CircularBuffer.h`: Lock-based circular buffer used for queues throughout.
- `fstorage.h` / `fstorage.cpp`: File-based data storage.
- `cJSON_wrap.h`: Wrapper around cJSON for config file parsing.

**`pi-library/`** — Hardware abstraction layer:
- `PiRobot` (PiRobot.h): Top-level hardware manager. Holds maps of providers, GPIOs, and items. Configured from a JSON file via `configure(file)`.
- **Providers** (`provider.h`, `GpioProvider.h`, `GpioProvider*.h`): Abstract GPIO controllers — SIMPLE (BCM direct), FAKE (emulation), MCP23008/MCP23017 (I2C expanders), PCA9685 (PWM), SPI.
- **Items** (`item.h`, `led.h`, `button.h`, etc.): Hardware peripherals (LEDs, buttons, motors, sensors, servos). Items reference GPIOs by name.
- Sensors: BMP280, MPU6050, Si7021, SGP30, TSL2561, MCP320X (ADC), DustSensor.
- Motors: DCMotor, DRV8835, DRV8825/DRV8834 (stepper), ULN2003 (stepper), Adafruit PWM servo.

**`pi-smachine/`** — Event-driven state machine:
- `StateMachine`: Thread-based event loop. Maintains a stack of `State` objects. Dispatches `Event` objects to the current state.
- `State`: Base class for application states. Override `OnEntry()`, `OnExit()`, `OnEvent(event)`.
- `StateFactory`: Abstract factory — subclass per application to provide `get_first_state()` and `get_state(name)`.
- `Timers2`: Timer support integrated with the state machine via SIGALRM.
- Key event types: `EVT_CHANGE_STATE`, `EVT_POP_STATE`, `EVT_FINISH`, timer events.

**`pi-env/`** — Application environment/configuration base class:
- `Environment` (Environment.h): Base class subclassed per application. Parses the JSON config for `context`, `http`, `email`, `mqtt`, `file` storage, and `sql` sections. Holds MQTT config (`_mqtt_conf`), file storage path, web port/root.

**`pi-http/`** — HTTP web server (Mongoose):
- `WebSettings` / `WebSettingsItf`: Base classes for serving web UI and handling HTTP endpoints. Applications subclass `WebSettings` to add custom routes.

**`pi-mqtt/`** — MQTT client:
- `MqttClient<T>`: Template client wrapping `MosquittoClient`. Threaded; internally queues publish messages and sends them when connected.
- `MosquittoClient`: Concrete implementation using libmosquitto.

**`pi-main/`** — Application bootstrap (header-only template):
- `PiMain<F, E, W>`: Glues `PiRobot`, `StateMachine`, a `StateFactory` (F), an `Environment` (E), and a `WebSettings` (W) together. Handles daemon/fork/debug run modes and signal handlers.

### Instantiating a New Application

```cpp
using tm_type = pimain::PiMain<MyFactory, MyEnvironment, MyWebSettings>;
int main(int argc, char* argv[]) {
    auto pmain = std::make_shared<tm_type>("myapp");
    pmain->load_configuration(argc, argv);
    pmain->run();
}
```

`MyFactory` extends `smachine::state::StateFactory`, `MyEnvironment` extends `smachine::env::Environment`, `MyWebSettings` extends `http::web::WebSettings`.

### Hardware Configuration JSON

See `pi-library/configuration.json` for a complete reference. Key sections:
- `"real_world"`: `true` for real hardware, `false` for emulation.
- `"providers"`: Array of GPIO/SPI provider definitions (type, name, I2C address, pins).
- `"gpios"`: Named GPIO pin assignments referencing providers.
- `"items"`: Hardware items (LEDs, buttons, motors, sensors) referencing GPIOs.
- `"context"`, `"mqtt"`, `"http"`, `"email"`, `"file"`: Runtime configuration parsed by `Environment::configure()`.

### `pi-core/`

Low-level BCM2835 access (DMA, PWM, mailbox). Only compiled when `REAL_HARDWARE` is set. Not used directly by application code.
