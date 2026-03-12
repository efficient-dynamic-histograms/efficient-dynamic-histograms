# Efficient Dynamic Histogram Algorithms for Real-Time Anomaly Detection in Data Streams

This repository contains the official implementation of the algorithms and the datasets used in the paper: "Efficient Dynamic Histogram Algorithms for Real-Time Anomaly Detection in Data Streams".

## 📋 Recommended requirements
- CMake >= 3.25
- C++20
- Python 3.14.2

## 🛠️ Installation
1. Clone the repository

```
git clone https://github.com/efficient-dynamic-histograms/efficient-dynamic-histograms.git
cd efficient-dynamic-histograms
```

2. Set up a virtual environment (Recommended)

```
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
```

3. Install dependencies

```
pip install -r requirements.txt
```

## 📊 Datasets and config files

```
├── archive/                     # Datasets and config files
│   ├── configs/                 # Config files for AUC and performance tests
│   ├── data/                    # Datasets used for AUC and performance tests
│   ├── distributions/           # Datasets and config files for goodness-of-fit tests with same $k$ and $B$ parameters
│   │   ├── configs/             # Config files
│   │   └── datasets/            # Dataset files
│   ├── distributions_memory/    # Datasets and config files for goodness-of-fit tests with fixed memory usage
│   │   ├── configs/             # Config files
│   │   └── datasets/            # Config files
```
