#SMART CONNECT
----

This library can help developers that want to work with **small devices** with C++, reusing the same code for all types of devices and components.

##Introduction

**SMART CONNECT** is a **layer** wrote in C++ for you to work amongst libraries that exist using the same structure of data, combined with an easy way to build new solutions.

*Pay atention: this library works like a layer for your solution.*

###Why you need use a thing like this?

Because your need to reduce development time. In so, your solutions come to life earlier.

###Devices Supported

* Copacabana
* Arduino
* ESP8266
* Intel Edison
* Intel Galileo GEN1,GEN2

##Setup and Install

You can install this lib in your enviroment with a few steps. See how to-do it below.

###Arduino

####OSX

* Get the zip of library.
* Go to menu Sketch > Include Library > Add Zip Library, choose your zip and ok.
* Now, check if the library are installed, menu Sketch > Include Library and scroll down to see if show **smartconnect**.
* If the library is installed, you are done.


##Methods

Below you can see all public methods in detail.

###void begin(int sp, bool debug);

Load the library to start.

```
/**
 * @param sp        [serial port]
 * @param smc_debug [active/inative debug mode]
 */
```

**Pay atention:** this method has to be executed first.

###bool connect(String hostname, char login[], char pass[], int port = 80);

Connect in the S.M.A.R.T platform.

```
/**
 * @param  hostname [hostname of platform]
 * @param  login    [user key]
 * @param  pass     [connect key]
 * @param  port     [port of hostname]
 * @return          [result of connection. true=connected,false=disconnected]
 */
```

###bool connected();

Check if a connection is estabilished.

```
/**
 * @return [a bool with state of connection. (true:connected/false:disconnected)]
 */
```

###String from(char* app, char* schema, char* path);

Public method to get any data from any application, see details about args below:

````
/**
 *
 * @param  app    [application name]
 * @param  schema [key of schema]
 * @param  path   [path of execution. can be the form name]
 * @return        [content of request]
 */
````

###String to(char* app, char* schema, char* path, char* stringData);

Public method to send data for an application, see details about args below:

```
/**
 * @param   app          [application name]
 * @param   schema       [key of schema]
 * @param   path         [path of execution. can be the form name]
 * @param   stringData   [data vars to send. ex: name=value]
 * @return               [content of request]
 */
```

###String exec(char* requestType, char* wreturn, char* app, char* schema, char* path, char* stringData = "");

Public method to exec a method in the core of an app.

```
/**
 * @param  requestType [type of request. (POST, GET)]
 * @param  wreturn     [return of request. (JSON/CSV)]
 * @param  app         [application name]
 * @param  schema      [key of schema]
 * @param  path        [path of execution. can be the form name]
 * @param  stringData  [data vars to send. ex: name=value]
 * @return             [description]
 * @return             [content of request]
 */
```

##API Details

The API documentation is avaiable on smartapps.com.br. ?????????????????????????? If you can see details about methods and returns of request. See the link below:

**Details about methods:**<br>
http://www.smartapps.com.br/a/docs/smart/api/metodos.html

##Examples

If you want see more details on how to use this library, go to the  **examples** folder.There are some files that may be of help.

* ESP8266
* Arduino

##Credits

José Wilker <jose.wilker@smartapps.com.br
