<div align="center">
  <img src="./docs/cachis.png" width="200" alt="Cachis" />
</div>
<h1 align="center">Cachis</h1>
<h2 align="center">An interactive cache simulator</h2>
<h3 align="center"><a>https://www.atc.unican.es</a></h3>

## About the project

Cachis is an interactive, trace-based simulator created for educational purposes. Its main features are:
* Memory and cache simulation with support for up to 10 levels of cache.
* Support for different replacement policies: LRU, FRU, RANDOM and FIFO.
* Support for Write-Back and Write-Through caches.
* Support for full, direct, and n-way associativity.
* Support for separated instruction and data caches.
* GTK4 GUI and alternative CLI only mode.
* Statisics generation with access count, miss, hit and access time statistics.
* Automatic DRAMSys trace generation.

## Getting started

### Prerequisites

Install the following dependencies. For Debian based systems:
```
apt-get install make gcc git libgtk-4-dev
```

Disclaimer: GTK 4.12 or above is required.

### Installation

1. Clone the repository:
```
git clone https://github.com/esteban-stafford/cachis.git
```

2. Run make:
```
cd cachis && make
```


## Usage

Output of `./cachis -h`:

```
Usage: cachis [OPTION]... <file>.ini [<file>.vca]
Simulate the memory hierarchy defined in <file>.ini.
Optionally provide a trace file. This overides the one specified in the ini file.
This a list of the options accepted:

  -i <file> Indicates which .ini file should be loaded.
  -e <file> Export a DRAMSys memory trace file to the specified file
  -g        Toggle GUI
  -h        Display this help and exit
  -v        Output version information and exit

Exit status:
 0  if OK,
 1  if configuration or other kind of errors.
```


By default Cachis will run in GUI mode. It is required to specify an .ini file with the -i flag. Please check the documentation for [.ini](./docs/INI.md) and [.vca](./docs/VCA.md) file formatting as well as the [full documentation](./docs/html/index.html) for further information
