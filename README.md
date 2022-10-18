# WWU Behavioral Neuroscience Wheel Tracker

## serial_watcher

Watches USB ports for serial devices that can accept data from mouse wheels.
Also watches USB ports for serial devices that transmit mouse wheel data.

## `wireless`

Code for transmitting data from the wheel to the private wireless access point.

### `client`

Code for a Wemos D1 R1 that watches for changes on its data pin, caches them, then sends POST requests to the server/AP.

### `server`

Code for another Wemos D1 R1 that hosts a HTTP server and acts as a private wireless access point for the client to communicate over.