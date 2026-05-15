# kvadraOSDesktopTeam4

A Linux C++ application that periodically scans a directory for media files and produces the result either as a JSON file or via a local HTTP endpoint.

## What it does

* Scans a target directory at a fixed interval.
* Detects media files by extension.
* Groups files into:

    * `audio`
    * `video`
    * `images`
* Produces JSON output with file names only.
* Supports two output modes:
    * JSON file output: `.media_files`
    * HTTP output: `GET http://localhost:1234/media_files`

## Tech stack

* C++20
* CLI11 for command-line parsing
* nlohmann/json for JSON serialization
* cpp-httplib for the local HTTP server

## CLI

### Basic usage

```bash
kvadraOSDesktopTeam4
```

Default behavior:

* scans the current user's home directory
* uses `30s` interval
* outputs to HTTP mode unless `-f` is enabled

### Options
```text
-h,--help                   Print this help message and exit
-p,--path TEXT [/home/ilya]
Root directory to scan
-i,--interval TEXT [30s]    Scan interval (e.g. 10s, 5min, 2h, 1d)
-r{false},--recursive       Disable recursive scan
-s,--symlinks               Follow symbolic links
-f,--file                   Save result to .media_files instead of serving it over HTTP.
-o,--output TEXT            Directory to save .media_files (defaults to --path)
--allow-ext TEXT ...        Allow only these extensions (e.g. .mp3 .wav)
--block-ext TEXT ...        Exclude these extensions (e.g. .tmp .txt)
```

### Examples

```bash
kvadraOSDesktopTeam4
Starting scan:
  Path: /home/ilya
  Interval: 30s
  Recursive: yes
  Symlinks: no
  Output JSON: http://localhost:1234/media_files
Press Ctrl+C or q to stop.


kvadraOSDesktopTeam4 -p /scan/path --block-ext .png -i 10m
Starting scan:
  Path: /home/ilya/test
  Interval: 10m
  Recursive: yes
  Symlinks: no
  Output JSON: http://localhost:1234/media_files
Press Ctrl+C or q to stop.


kvadraOSDesktopTeam4 -p /scan/path --block-ext .flac -i 1h -o /result/path -f -r -s
Starting scan:
  Path: /home/ilya/test
  Interval: 1h
  Recursive: no
  Symlinks: yes
  Output JSON: /home/ilya/test/.media_files
Press Ctrl+C or q to stop.


```


## Build

```bash
cmake -S . -B build
cmake --build build
```

## CMake dependencies

The project fetches the following libraries with `FetchContent`:

* `nlohmann/json`
* `CLI11`
* `cpp-httplib`

## Output format

Example:

```json
{
  "audio": ["111.mp3", "222.wav"],
  "video": ["333.mpg"],
  "images": ["444.jpeg", "555.png"]
}
```
