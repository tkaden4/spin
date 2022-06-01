import EventEmitter from "events";
import { watchDrives, WatchDrivesOpt } from "./storage";

export class DriveManager {
  private _cancel = () => {};
  private _drives: Record<string, string> = {};
  private _update: () => Promise<void> = async () => {};
  private _emitter: EventEmitter = new EventEmitter();

  constructor(private watchOpts: Exclude<WatchDrivesOpt, "onDriveChange"> = {}) {}

  private onUpdate(drive: string, path: string | undefined) {
    if (path === undefined) {
      delete this._drives[drive];
      this._emitter.emit("remove", drive, path);
    } else {
      this._drives[drive] = path;
      this._emitter.emit("update", drive, path);
    }
  }

  async watch() {
    [this._cancel, this._update] = await watchDrives({
      ...this.watchOpts,
      onDriveChange: (drive, path) => {
        this.onUpdate(drive, path);
      },
    });
  }

  stop() {
    this._cancel();
    this._cancel = () => {};
  }

  async available(): Promise<string[]> {
    await this._update();
    return Object.values(this._drives);
  }

  async active(path: string): Promise<boolean> {
    await this._update();
    return Object.values(this._drives).includes(path);
  }

  get events() {
    return this._emitter;
  }
}
