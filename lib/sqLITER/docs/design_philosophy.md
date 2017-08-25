# sqLITER v0.6, a Thin C++ Wrapper for SQLite

## Goals

sqLITER was started as a personal project to provide database capabilities in several small local and server applications.  The goal was to make application use as easy as possible and to allow me to push myself up the SQLite learning curve by encapsulating the code such that when I discovered errors or possible improvements code would only have to be updated in the wrapper.  A closely related goal was to minimize boilerplate code for simple database access.

After several iterations involving object models including discrete database, statement, and other objects, I arrived at the present design which presents a very simple interface to the user.  More complicated applications may be built up from multiple instances, such as when certain SQL statements must be repeatedly executed and the overhead of compiling the SQL each time should be avoided.

I hope this library is helpful to others in making a quite advanced database (SQLite) more accessible.

-Nathan

## Summary

SQLite is an excellent and respected embedded database library written in C.  It is useful on a wide variety of platforms and applications, and is prized for its simplicity of installation, serverless operation, and its resistance to data corruption.  The interface is a traditional C style API giving an expansive list of capabilities and ways in which to use and optimize database functions.  

Use of SQLite in an application context requires a good knowledge of databases and the use of either repititious boilerplate code or a wrapper.  sqLITER attempts to provide a basic and  accessible C++ wrapper for the SQLite library.  The emphasis is upon simplicity and robustness.  The access is higher than the ODBC level and is intended to eliminate significant quantities of boilerplate code.
The intended sqLITER goals are the same end-uses as SQLite itself.  Quoting from the SQLite documentation: "SQLite is not designed to compete with Oracle. SQLite is designed to compete with fopen()."

## Key Features

sqLITER is a thin wrapper for the excellent SQLite library.  It is a small and efficient C++ SQL database library that includes support for nearly all of the SQL92 Standard.  It does not require a separate SQL server process, and can be easily added to a new or existing application.  

sqLITER is suitable for a wide range of applications that range from embedded devices through web servers.  

## Key Comparisons

Several metrics suggest themselves to compare sqLITER with other database tools.  For the purpose of the stated metrics, sqLITER is essentially equivalent to SQLite itself for common use cases.  The use of several standard C++ libraries such as vectors and strings gives additional type and bounds safety at the expense of the usual small C++ overhead vs straight C.  For normal development, this should not be a significant issue.

Except as otherwise stated, any tradeoffs are a result of SQLite itself; sqLITER is a thin C++ wrapper rather than a new library complete with its own from-scratch design decisions.

**Ease of Implementation.** sqLITER consists of a pair of header files and a single code files (.h, .cpp), one of each being the underlying SQLite engine.  Adding sqLITER to a software project is as easy as including those widely compatible files and compiling.  There's no separate server to install and configure either for developers or for deployment.  

**Data Integrity (ACID).**  With respect to reliability and data loss, SQL databases are generally measured against a standard called ACID: in short, actions to update the database should succeed or fail without undefined or unexpected side effects of failure.   Data is not corrupted by making a partial rather than full update, such as updating a first name of a person in a table without updating the last name because of a power interruption.  Backups are required in the event of a hard drive failure, but the SQL database itself is highly resistant to corruption even when called in an improper or unexpected way.

**Performance.**  sqLITER is based upon SQLite, which treasures data intregity above all else.  In order to achieve "What happens if someone unplugs the computer halfway through a transaction" reliability, the maximum transaction rate for this architecture is limited by the rotational rate of the hard disk.  At 7200 RPM, that corresponds to about sixty transactions per second.  Note that a single transaction may consist of thousands of individual updates/inserts.  Pure database reads do not count against this, so even an active webserver can use sqLITER to maintain both session and web service data.  If necessary, a developer can choose to optimize by means such as using asyncronous instead of syncronous hard disk writes to achieve much higher transaction rates.

**SQL vs NOSQL solutions** such as BigTable, Hadoop, BerkeleyDB, etc.:  sqLITER falls entirely toward the end of full SQL features along with other SQL tools such as Oracle, PostgreSQL, MySQL, etc.  This also relates to the decision to emphasize data integrity over performance.

**Compromises to support "Big Data"** such as Oracle, Google's BigTable, etc.  sqLITER can theoretically support databases up to 140 Terabytes though practical limitations come into play far before that limit.  It is recommended for much smaller databases than that, in particular those which leave space on the same physical hard drive equal to their own size, which safely allows SQL VACUUM commands to be executed. Degradation is gradual rather than catastrophic, enabling database tuning and optimization as appropriate.

**Simultaneous Users.**  sqLITER can support an arbitrary number of simultaneous users.  Note restrictions on transaction rates.  It is not intended to scale to the level of Google for search, Schwab for stock trades, or Amazon for purchases.  However, it is more than adequate for most companies up until the point that traffic and revenue justifies a replacement.  At that point, the adherence to SQL standards makes a transition easy.

**Permissions.** sqLITER exists as part of the application which uses it, not as part of a separate client/server architecture.  Permissions are only restricted by any operating system file restrictions.

##Design

sqLITER provides an easy-to-use and object-oriented wrapper over SQLite.  While C++, it maintains the API-style of access with return values.  The design stresses simplicity and testable correctness over optimizations and complex features with complicated interactions.  The API is also intended to fit well with normal database operations to facilitate a migration to a different underlying database if necessary.

Testable correctness in this context means that the provided library functions make it easy to use the library correctly and assure that the consequences of using a call incorrectly are both minimal and easily detected in both test cases run using the library and actual applications.

A single method of doing things is preferred to multiple methods.  For this reason, arguments to SQL statements are always bound by name and not by order within the statement.  It is the most flexible method and also avoids several types of coding errors which may occur during revision of SQL statements.
The wrapper is intended to apply to strict database functions only.  There would be no point to wrapping standalone utility functions provided by SQLite such as hex, datetime, etc.

While an effort has been made to preserve reasonable speed and memory characteristics, a deliberate decision to accept overhead where it yields tangible benefits to robustness has been accepted.  An example is single calls to return multiple rows of results and providing memory for each possible type of result (text, integer, float, blob) in each result field.  Rather than raw pointers to char or unsigned char, std::string and std::vector are used for TEXT and BLOB types respectively.

SQLite itself supports 64 bit integers and 8 byte doubles.  Rather than attempting to provide separate calls for different types of values (int, long, etc.), the calls in and out assume the largest type supported by SQLite.  When passing values in, this results in a safe implicit cast.  When passing values back out of the database, it is the responsibility of the calling application to know whether assigning/casting to other types is appropriate.  While not ideal, this does reflect the spirit of SQLite, which doesn't even enforce the general type of a cell (such as TEXT vs FLOAT). 

SQLite does not strictly enforce types within cells.  A table may be created with a given column with a nominal type of floating point.  Subsequently, a user may store values of any type in that field: float, integer, or even text.  Thus, a given column in the database does not necessarily consist of identical types.  For this reason, combined with the automatic type conversion/coercion performed by C++, sqLITER relies upon separate calls to bind each database type: bindint, bindfloat, etc.  Using a single call name with multiple signatures could result in incorrect types being inserted.  For example, a series of bind(int foo), bind(float foo), etc. would rely on the user being absolutely consistent in coding when writing code such as bind(1) vs bind (1.0).

Use of a database relies on one primary class which represents the entire database including the state of the transaction currently being processed .  Several other minor classes represent data and state.
No analysis of the SQL code is made by the sqLITER wrapper; no distinction is made at this level between the various types (or even number) of actual SQL commands embedded in a statement object.  This is generally safe, such as beginning and ending transactions.  A consequence is that generally speaking only one command which produces output should be made per runstatement() call.  While a limitation to a single final SELECT is obvious, this also includes INSERTS in which the ID of the new row needs to be checked via lastinsertrowid().

While best practice is to check return values of each call providing them, failing to detect an error condition in the application using the sqLITER library will not result in the library causing a crash via means such as invalid pointer dereferencing.  Instead, further calls will fail gracefully, such as during an attempt to execute statements in a database which wasn't successfully opened.

Implementation is restricted to basic C++ language features for simplicity and maximum compatibility.  For example, support for identifying variable numbers of expected result types in one call, pushvaltypesout(), is implemented via multiple function definitions rather than through use of C/C++'s va_list, templates, or the features introduced in C++11.  std::vector and std::string are used extensively for input and output on the grounds that they provide safer and more friendly containers than raw arrays.