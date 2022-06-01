import fs from "fs";
import { TrackingDB } from ".";

export function fileDB(path: string, append: boolean = false): TrackingDB {
  if (!append) {
    fs.writeFileSync(path, "");
  }
  return {
    async insert(entry) {
      fs.appendFileSync(path, `${entry.timestamp}\t${entry.wheel_id}\n`, {});
    },
  };
}
