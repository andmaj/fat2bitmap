# fat2bitmap

*Creates a bitmap from FAT file system free/used clusters. The bitmap is in text format so contains zero (character 48) and one (character 49) bytes. A zero means that the cluster is free, a one means that the cluster is used.*

For example creating a usage map of FAT file system (from filesys.iso image file):
```fat2bitmap --if filesys.iso --of usagemap.txt```

usagemap.txt will contain text:

1110111100001100110 .. and so on.

## Usage

```fat2bitmap [options]```

**Options:**

* **--if FILE**
Specifies the input file.
* **--of FILE**
Specifies the output file (default: stdout).
* **--help**
Prints usage information.
* **--version**
Prints version information.

Program does support output to stdout (so redirection is possible), but cannot get its input data from stdin.

You can get progress information by sending a SIGUSR1 signal to the process:
```kill -SIGUSR1 pid``` (where pid is the process identifier)

### Example 1
Make cluster map of /dev/sda1 partition (FAT):

```fat2bitmap --if /dev/sda1 --of mapsda1.txt```

### Example 2
Make cluster map of /dev/sdb3 partition (FAT) and create a PBM image of it:

``` fat2bitmap --if /dev/sdb3 | bitmapdd --bs 1 --null 48 | bitmap2pbm --of mapsdb3.pbm ```

For these tools check the "See also" section.

## Build
Program should build on any UNIX like operation system with a standard C compiler and make utility.

To compile:
```
make
```

To compile without signal handling:
```
make CFLAGS='-DNOSIGNAL'
```

To run:
```
./bin/fat2bitmap
```
(however you must give at least --if parameter to it)

To clean:
```
make clean
```

## Author

Written by Andras Majdan.

License: GNU General Public License Version 3

Report bugs to <majdan.andras@gmail.com>

Heavily based on code of [dosfstools](http://daniel-baumann.ch/software/dosfstools/). Without this great tool it would have taken ages to write this program.

## See also

[bitmap2pbm](https://github.com/andmaj/bitmap2pbm) - Creates a P4 type PBM image from a binary file.

[bitmapdd](https://github.com/andmaj/bitmapdd) - Creates a bitmap from a file (or device).
