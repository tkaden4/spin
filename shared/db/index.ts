import { TrackingEntry } from "..";

export interface TrackingDB {
  insert(entry: TrackingEntry): Promise<void>;
}
