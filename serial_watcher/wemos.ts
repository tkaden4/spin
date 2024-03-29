import { ReadlineParser } from "@serialport/parser-readline";
import { SerialPort } from "serialport";
import { EventEmitter } from "stream";

export type WemosMessage = { type: "log"; log: string } | { type: "data"; wheel: string; count: number };

export namespace wemos {
  export function fromPath(baud: number, ttyPath: string) {
    const port = new SerialPort({
      baudRate: baud,
      path: ttyPath,
    });
    const parser = port.pipe(new ReadlineParser({ delimiter: "\r\n", includeDelimiter: false }));
    return {
      port,
      parser,
    };
  }

  export function parseMessage(data: string): WemosMessage {
    try {
      const { wheel, rotations } = JSON.parse(data);
      if (rotations === undefined || wheel === undefined) {
        return { type: "log", log: data };
      }
      const count = Number.parseInt(rotations);
      return {
        type: "data",
        wheel,
        count,
      };
    } catch {
      return { type: "log", log: data };
    }
  }
}

export interface WemosConfig {
  baudRate: number;
  interval: number;
}

export class WemosWatcher {
  private _emitter: EventEmitter = new EventEmitter();
  private _ports: Record<string, ReturnType<typeof wemos.fromPath>> = {};
  private _cancel: () => void = () => {};
  private _interval: number;
  private _baudrate: number;

  constructor({ interval = 1000, baudRate = 115200 }: WemosConfig) {
    this._interval = interval;
    this._baudrate = baudRate;
  }

  watch() {
    const interval = setInterval(async () => {
      const drives = await SerialPort.list();
      for (const drive of drives) {
        if (drive.manufacturer !== "1a86" || drive.productId !== "7523") continue;
        if (!(drive.path in this._ports)) {
          const port = (this._ports[drive.path] = wemos.fromPath(this._baudrate, drive.path));
          port.port.on("end", () => console.log(port.port.path, "end"));
          port.port.on("error", (e) => console.error(e));
          port.parser.on("data", (data: string) => {
            const message = wemos.parseMessage(data);
            if (message.type === "data" && message.count === 0) {
              return;
            }
            this._emitter.emit("message", message);
          });

          this._emitter.emit("added", drive.path);
          port.port.on("close", () => {
            this._emitter.emit("removed", drive.path);
            delete this._ports[drive.path];
          });
        }
      }
    }, this._interval);
    this._cancel = () => {
      clearInterval(interval);
    };
  }

  stop() {
    for (const key in this._ports) {
      try {
        this._ports[key].port.close();
      } catch {}
      delete this._ports[key];
    }
    this._cancel();
  }

  get events() {
    return this._emitter;
  }
}
