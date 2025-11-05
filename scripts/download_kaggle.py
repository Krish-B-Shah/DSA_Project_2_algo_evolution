#!/usr/bin/env python3
"""
Download the Kaggle dataset and copy CSVs into data/logs/ for the viz.

Requires:
  pip install kagglehub

Usage:
  python3 scripts/download_kaggle.py
"""
import os
import shutil
from pathlib import Path

def main():
    try:
        import kagglehub
    except ImportError:
        print("ERROR: kagglehub not installed. Run: pip install kagglehub")
        return 1

    path = kagglehub.dataset_download("bekiremirhanakay/benchmark-dataset-for-sorting-algorithms")

    src = Path(path)
    dest = Path(__file__).resolve().parents[1] / 'data' / 'logs'
    dest.mkdir(parents=True, exist_ok=True)

    copied = 0
    for p in src.rglob('*.csv'):
        target = dest / p.name
        print(f"Copying {p} -> {target}")
        shutil.copy2(p, target)
        copied += 1

    print(f"Done. Copied {copied} CSV files to {dest}")
    return 0

if __name__ == '__main__':
    raise SystemExit(main())


