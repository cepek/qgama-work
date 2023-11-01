# QGama project ChangeLog

```QGama``` is a Qt based user
interface for GNU Gama adjustment of geodetic networks in which XML
input is complemented with a database. The most simple case is Sqlite3
file database, which does not need any special knowledge from the user.

http://www.gnu.org/software/gama/qgama

## 2.06.1

New build with Qt version 6.6.0

* The only major change is build with higher version of Qt.
* New PowerShel script `deploy-qgama-2.06.1.ps1` in directory `deploy-win64`. 
  Two older deploy *.bat files are still kept there.
* Builds for Windows are produce by QtCreator, with Visual Studio used as a generator
  and `windeployqt.exe` for creating binary distribution (*.zip file).
* Several other minor changes.
* `ChangeLog.md` file added.

## 2.06

Build with Qt version 6.5.3 (Linux) and 6.2.2 (Windows)

* Add coefficient to the qgama plain text output
* Set output precision for fixed coordinates to 5 digits