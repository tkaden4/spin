import path from "path";
import { getCurrentEntry, TrackingEntry } from "../shared";
import { fileDB } from "../shared/db/file";
import { DataQueue } from "./data";
import { DriveManager } from "./drives";
import { WemosMessage, WemosWatcher } from "./wemos";

async function main() {
  const entriesFile = "entries.log";
  const logFile = "log.log";

  const drives = new DriveManager({
    interval: 300,
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

  drives.events.on("interval", async () => {
    if ((await drives.available()).length === 0) {
      console.log("queue locked");
      queue.lock();
    } else {
      console.log("queue unlocked");
      queue.unlock();
    }
  });

  drives.events.on("update", async (_drive, path: string) => {
    console.log(_drive, path);
  });

  queue.lock();
  await drives.watch();

  const wemosWatcher = new WemosWatcher({ interval: 1000, baudRate: 115200 });
  wemosWatcher.events.on("added", (wm) => {
    console.log(`wemos device ${wm} added`);
  });
  wemosWatcher.events.on("removed", (wm) => {
    console.log(`wemos device ${wm} removed`);
  });

  wemosWatcher.events.on("message", (message: WemosMessage) => {
    if (message.type === "log") {
      console.log(message.log);
    } else {
      queue.insert(getCurrentEntry(message.wheel, message.count));
    }
  });

  wemosWatcher.watch();
}

async function run() {
  while (true) {
    try {
      await main();
    } catch (e) {
      throw e;
      console.log(e);
    }
  }
}

run();
