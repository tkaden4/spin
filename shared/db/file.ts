import fs from "fs";
import { TrackingDB } from ".";

export function fileDB(path: string, append: boolean = false, tsv = true): TrackingDB {
  if (!append) {
    fs.writeFileSync(path, "");
  }
  return {
    async insert(entry) {
      fs.appendFileSync(path, `${entry.timestamp}\t${entry.wheel_id}\t${entry.id}\t${entry.rotations}\n`, {});
    },
  };
}
