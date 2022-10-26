import fs from "fs";
import { TrackingDB } from ".";

export function fileDB(path: string, append: boolean = false, csv = true): TrackingDB {
  if (!append) {
    fs.writeFileSync(path, "");
  }
  return {
    async insert(entry) {
      fs.appendFileSync(path, `${entry.timestamp}, ${entry.wheel_id}, ${entry.id}, ${entry.rotations}\n`, {});
    },
  };
}
