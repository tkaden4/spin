import * as drives from "drivelist";
import * as path from "path";
import { TrackingEntry } from "../shared";
import { TrackingDB } from "../shared/db";
import { fileDB } from "../shared/db/file";

export type WatchDrivesOpt = {
  interval?: number;
  ignore?: (drive: drives.Drive) => boolean;
  onDriveChange?: (device: string, path?: string) => void;
};

export async function watchDrives({ interval = 1000, onDriveChange = () => {}, ignore = () => false }: WatchDrivesOpt) {
  let driveStore: Record<string, string> = {};

  const update = async () => {
    const currentDrives = (await drives.list()).filter((x) => !ignore(x));
    currentDrives.forEach((drive) => {
      const oldPath = driveStore[drive.device];
      const newPath = drive.mountpoints[0]?.path;
      if (newPath !== oldPath) {
        driveStore[drive.device] = newPath;
        onDriveChange(drive.device, newPath);
      }
    });
    for (const drive in driveStore) {
      if (!currentDrives.map((x) => x.device).find((x) => x === drive)) {
        onDriveChange(drive, undefined);
        delete driveStore[drive];
      }
    }
  };
  const intervalId = setInterval(update, interval);
  return [
    () => {
      clearInterval(intervalId);
    },
    update,
  ] as const;
}

export type BroadcastStorageOpts = {
  ignore?: string[];
  ignorePath?: string[];
  path: string;
  log?: boolean;
};

export class BroadcastStorage implements TrackingDB {
  constructor(private opts: BroadcastStorageOpts) {}

  async insert(entry: TrackingEntry): Promise<void> {
    const currentDrives = (await drives.list())
      .filter((drive) => !this.opts.ignore?.includes(drive.device))
      .filter((x) => x.mountpoints.length)
      .filter((x) => !this.opts.ignorePath?.includes(x.mountpoints[0].path))
      .filter((x) => !x.isSystem)
      .filter((x) => x.busType === "USB");
    for (const drive of currentDrives) {
      if (this.opts.log) {
        console.log(`${drive.mountpoints[0].path}\t${entry.timestamp}\t${entry.wheel_id}`);
      }
      await fileDB(path.join(drive.mountpoints[0].path, this.opts.path), true).insert(entry);
    }
  }
}
