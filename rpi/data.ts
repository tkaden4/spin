export class DataQueue<T> {
  private _locked: boolean = false;
  private _cache: T[] = [];

  constructor(private onFlush = (_data: T[]) => {}, private maxSize = 2048) {}

  lock() {
    this._locked = true;
  }

  unlock() {
    if (!this._locked) return;
    this.onFlush(this._cache);
    this._locked = false;
    this._cache = [];
  }

  insert(entry: T) {
    if (this._locked) {
      this._cache.push(entry);
    } else {
      this.onFlush([entry]);
    }
  }
}
