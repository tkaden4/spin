import path from "path";
import { getCurrentEntry, TrackingEntry } from "../shared";
import { fileDB } from "../shared/db/file";
import { DataQueue } from "./data";
import { DriveManager } from "./drives";
import { trackRotations } from "./tracker";

async function main() {
  console.log("watcher started");
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
  // const onDisk = fileDB(untildify("~/.entries.log"), true);

  await trackRotations(14).forEach(() => {
    queue.insert(getCurrentEntry("wheel:14"));
  });
}

async function run() {
  while (true) {
    try {
      await main();
    } catch (e) {
      console.log(e);
    }
  }
}

run();
