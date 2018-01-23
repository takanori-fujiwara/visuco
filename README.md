## Visuco - A Visual Analytics System for Optimizing Communications in Massively Parallel Applications

About
-----
* Visual analytics software for identifying bottlenecks on prallel communications used MPI and finding better communication options

* This project is a work for the paper below.     
***A Visual Analytics System for Optimizing Communications in Massively Parallel Applications***    
Takanori Fujiwara, Preeti Malakar, Venkatram Vishwanath, Khairi Reda, Michael E. Papka, and Kwan-Liu Ma    
In Proceedings of IEEE Symposium on Visual Analytics Science and Technology (VAST), 2017.

* Features
  * Enable to visualize large scale communications (8,000 computers nodes and more)
  * Three different type of views to identify communication bottlenecks
  * Alternative better route suggestion

* Dataset
  * Dataset is available from https://github.com/takanori-fujiwara/par-comm-data

Requirements
-----
* Note: Tested on Mac OS X (El Capitan, Sierra, and High Sierra).
* XCode (latest)
* Qt5 (latest)
* QtCreator (latest)
* (Follows are required if you want to generate new graph data.)
 * Python 2.7 or higher
 * graph-tool (latest)
 * R (latest)

Installation
-----
#### Mac OS
* Install latest XCode from Mac App store
* After finishing installation of XCode, __launch XCode at once to verify XCode__.
* Install Qt. If you use home brew(http://brew.sh) use the commands below

    `brew update`

    `brew install qt5`

* Check qmake version.

    `qmake -v`

    If the version is lower than qt5, use the command below.

    `brew link --force --overwrite qt5`

* Download and Install QtCreator.

    `brew tap caskroom/cask`

    `brew cask install qt-creator`

##### Follows are in case you want to generate new graph data
* Install R and R packages.

    `brew install r`

  Check `R` runs the same version installed above.

    `which R`

  If this does not return `/usr/local/bin/R`, add `export PATH=/usr/local/bin:$PATH` in .bash_profile. Also, try to `brew unlink r && brew link r`.

    `R` (run R in terminal)

    `>>> install.packages(c("sna", "argparse"))`

* If you have not installed python, install python

    `brew install python`

  Check `python2` runs the same version installed above.

    `which python2`

  If this does not return `/usr/local/bin/python2`, add `export PATH=/usr/local/bin:$PATH` in .bash_profile. Also, try to `brew unlink python && brew link python`.  


* Install graph-tool. See this requirements and installation. [instruction](https://graph-tool.skewed.de/).

    -   Note: current graph-tool version cannot been installed via homebrew since homebrew/science was deprecated. Try a different way.

    <!-- -   Note: current graph-tool version in homebrew does not work with the latest "boost". Try to use the command below.

    	`brew install graph-tool --build-from-source` -->

    -   After installing graph-tool, check whether it works or not.

    	`python2` (run python2 in terminal)

   		`>>> from graph_tool.all import *`

* Build the software with the procedures below.

	- Launch "./src/qtCode/visuco/visuco.pro" with QtCreator.

	- If QtCreator says no kits, select "options" link.

		- Configure Kits for Build. go to "preference -> Qt Versions" and then add "/usr/local/bin/qmake" if there is no qt version.
		- Then, go to "Kits->Desktop" and select added latest qt version in "Qt version", select Clang(x86 64bit) in "Compiler".

		- After finishing above setting, select "Desktop" as a Kit.

	- Built with QtCreator. "Release" mode is recommended for better performance.

Usage
-----
##### Initial Settings
* Run from QtCreator or launch "visuco.app" in build directory.
* Select "Preferences" in the menu bar and set paths for python and RScript commands.

******

##### Generate new analysis data including graph information from route files and mappind data
* Select "Generate Analysis Data" from "File" menu.
* Select a route file (TXT) that you want to analyze. A route file must be the same format with an output file from bgqroute (https://github.com/pmalakar/bgqroute).

	For example,

    	Hop 1: [6-12] 6 (0 0 1 1 0 0) -> 4 (0 0 1 0 0 0)
    	Hop 2: [6-12] 4 (0 0 1 0 0 0) -> 12 (0 1 1 0 0 0)
    	...

* Select a matrix mapping file (TXT file with space separation). This is used for mapping the coordinates of ranks to the coordinates in the matrix view.

	The format must be

    	1st line: size of each dimensions
    	2nd line: Permutation order in the matrix view as you want to use.
        3rd line: Network topology type. Currently, we support only "5dtorus" and "theta". Select one of them.

    For example, if you use 5D torus with 4x2x4x8x2 sizes (each number is cooresponding to A,B,C,D,E dimension) and EDCBA order

    	4 2 4 8 2
    	4 3 2 1 0
        5dtorus

* (Optional) Select a hop-byte file (TXT file with space separation). This is used when the system suggests alternative routes or mapping.

	The format must be

    	Route Source,Destination,Message Size,Number of Hops

	For example,

    	4 10 342321 3

* Set number of cores according to your environment that used for obtaining the route file.

* Set a threshold for the number of nodes, which is used for graph partitioning and aggregating the node.

* Finally, set an output directory as you want and push "OK". Then, wait for a while until finishing running all scripts. (We have a plan for implementing notification of finishing running.)

******

##### Load generated new analysis data
* Select "Load Analysis Data" from "File" menu.
* Select the directory you generated in the "Generate new analysis data" step.
******

## How to Cite
Please, cite:
* Takanori Fujiwara, Preeti Malakar, Venkatram Vishwanath, Khairi Reda, Michael E. Papka, and Kwan-Liu Ma, "A Visual Analytics System for Optimizing Communications in Massively Parallel Applications." In Proceedings of IEEE Symposium on Visual Analytics Science and Technology (VAST), 2017.
