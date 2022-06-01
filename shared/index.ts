export interface TrackingEntry {
  timestamp: string;
  wheel_id: string;
}

export function getCurrentEntry(wheel_id: string): TrackingEntry {
  return {
    wheel_id,
    timestamp: `${Date.now()}`,
  };
}
