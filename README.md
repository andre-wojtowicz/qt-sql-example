# Qt SQL Example

An exemplary [Qt](https://www.qt.io/) app that connects to SQL server and displays a table from a database. The program was made for educational purposes.

Compiled Windows and Linux applications can be downloaded from [Releases](https://github.com/andre-wojtowicz/qt-sql-example/releases).

![](screenshot.png)

The program was tested on: 
 * clients: Windows 10 with Qt 5.9.1, Ubuntu 16.04 with Qt 5.5.1, 
 * servers: MySQL Server 5.5.57, Microsoft SQL Server 2008 R2 SP3.

## Client config

The program reads `config.ini` on startup. The file must be located in the app working directory.

## SQL database drivers

A comprehensive discussion about SQL database drivers (Qt driver plugins as well as system drivers) can be found in [Qt Documentation](https://doc.qt.io/qt-5/sql-driver.html). MSSQL is handled by the ODBC driver.

### Windows 10

* Both Qt SQL driver plugins (QMYSQL and QODBC) should be already bundled with Qt SDK.
* MySQL driver can be *installed* by placing `libmysql.dll` from [C API for MySQL (mysqlclient)](https://dev.mysql.com/downloads/connector/c/) in the app working directory.
* ODBC Driver Manager and SQL Server driver are built in; hence, no further installation is necessary. Alternatively, instead of SQL Server driver, one can use Microsoft ODBC Driver; see [Microsoft Docs](https://docs.microsoft.com/en-us/sql/connect/odbc/windows/microsoft-odbc-driver-for-sql-server-on-windows).

### Ubuntu 16.04

Packages with Qt driver plugins and system SQL drivers:

* MySQL - `libqt5sql5-mysql` `libmysqlclient20`,
* ODBC ([unixODBC Driver Manager](http://www.unixodbc.org) and Microsoft ODBC Driver) - `libqt5sql5-odbc` `libodbc1` `unixodbc` `odbcinst` `msodbcsql`__*__.

__*__ How to install `msodbcsql` - [Installing the Microsoft ODBC Driver for SQL Server on Linux and macOS](https://docs.microsoft.com/en-us/sql/connect/odbc/linux-mac/installing-the-microsoft-odbc-driver-for-sql-server).
