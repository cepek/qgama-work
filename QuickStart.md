# QGama (GNU Gama Qt GUI)

_ Aleš Čepek &lt;cepek@gnu.org&gt; and Petra Millarová &lt;millapet@gnu.org&gt;_

GNU Gama is a project dedicated to adjustment of geodetic networks. It
is intended for use with traditional geodetic surveyings which are
still used and needed in special measurements (e.g., underground or
high precision engineering measurements) where the Global Positioning
System (GPS) cannot be used.

http://www.gnu.org/software/gama/

Qt GUI for geodetic networks is an application `qgama` in which XML
input is replaced by a database. The most simple case is `Sqlite3` file
database, which does not need any special knowledge from the user.

Download and build
------------------

Download source codes from git server and run the following commands
(for example in Linux Terminal/Bash or Windows/PowerShell)

    git clone git://git.sv.gnu.org/gama/qt.git  qgama        # explicit name
    cd  qgama
    git clone https://git.savannah.gnu.org/git/gama.git      # checkout a version if needed
    mkdir build
    cd build

Adjust as needed and select

    cmake .. -GNinja -DCMAKE_PREFIX_PATH=~/Qt/6.2.2/gcc_64/         # Linux
    cmake .. -GNinja -DCMAKE_PREFIX_PATH=C:/Qt/6.2.2/msvc2019_64/   # Windows

If the `CMAKE_PREFIX_PATH` is defined in the `CMakeLists.txt` then simply
run

    cmake .. -GNinja  # both Linux or Windows

Finish the build and run `qgama`

    cmake --build .
    ./qgama &


How to build an example database
--------------------------------

Probably you will also want an example database. You can easily build
`Sqlite3` database from GNU Gama example collection. To create the
database you will also need some programs from GNU Gama project.

    $ mkdir _your-working-directory_
    $ cd _your-working-directory_

Now return to _your-working-directory_, download examples and build
the examples.db database (_Sqlite3_).

    $ cd ..
    $ git clone git://git.sv.gnu.org/gama/examples.git
    $ ls
    examples  gama
    $ cd examples/gama-local/sqlite/
    $ make build-sqlite3-examples.db
    $ ls
    examples.db  Makefile

TODO
----

* import yaml format
* update online help
