import path from "path";
import { TrackingEntry } from "../shared";
import { fileDB } from "../shared/db/file";
import { DataQueue } from "./data";
import { DriveManager } from "./drives";

async function main() {
  const entriesFile = "entries.log";
  const logFile = "log.log";

  const drives = new DriveManager({
    interval: 500,
    ignore: (drive) =>
      !drive.isRemovable ||
      drive.isSystem ||
      drive.busType !== "USB" ||
      drive.isReadOnly ||
      drive.mountpoints.length === 0 ||
      drive.mountpoints?.[0]?.path === "G:\\",
  });

  const queue = new DataQueue<TrackingEntry>(async (data) => {
    (await drives.available()).forEach(async (drive) => {
      console.log(drive, data.length);
      const db = fileDB(path.join(drive, entriesFile), true);
      for (const entry of data) {
        await db.insert(entry);
      }
    });
  });

  drives.events.on("remove", async () => {
    if ((await drives.available()).length === 0) {
      queue.lock();
    }
  });

  drives.events.on("update", async (_drive, path: string) => {
    if ((await drives.available()).length === 1) {
      queue.unlock();
    }
  });

  queue.lock();
  await drives.watch();

  setInterval(() => {
    queue.insert({
      timestamp: new Date().toUTCString(),
      wheel_id: "wheel:1",
    });
  }, 500);
}

main();
