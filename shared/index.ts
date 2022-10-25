export interface TrackingEntry {
  timestamp: string;
  id: string;
  wheel_id: string;
  rotations: number;
}

export function getCurrentEntry(wheel_id: string, rotations: number): TrackingEntry {
  const now = Date.now();
  return {
    wheel_id,
    id: `${now}`,
    timestamp: new Date(now).toUTCString(),
    rotations,
  };
}
