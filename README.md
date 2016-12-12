#The Nial Language


#Introduction

The Nial language was developed by Mike Jenkins and Trenchard More in a collaborative research project 
supported by Queen's University at Kingston and IBM Cambridge Scientific Center from 1979 to 1982. Mike's 
team at Queen's designed and implemented a portable C-based interpreter written, Q'Nial that was 
initially released in 1983.

The language combines Trenchard More's theory of nested arrays with Mike Jenkins' ideas on how to 
build an interactive programming system. The goal was to combine the strengths of APL array-based 
programming with implementation concepts borrowed from LISP, structured programming ideas from 
Algol, and functional programming concepts from FP. THe interpreter, originally developed for 
Unix, was small enough to run on the then newly released IBM PC and portable enough to execute 
on IBM mainframes computers. 

Nial Systems limited licensed the interpreter from Queen's Uinversity and marketed it widely. 
Mike Jenkins continued to refine both the language and its implementation. In 2006 Mike released 
Version 6.3 as an open source project to encourage continued development of Nial.

In 2014 MIke started working with John Gibbons to develop a 64-bit version and to add 
capabilities that John needed for his work. The decision was made to target the open 
source for Unix-based platforms  and release it on GitHub.  


#Version 7 Q'Nial

This version of Q'Nial is intended for people who want to integrate the functionality of Nial into 
projects that can take advantage of its powerful array computations for  numeric, symbolic, or 
data analysis problems. The major changes are:

-   the capability to build both 32 and 64 bit versions
-   the removal of multi-platform support and a focus on Unix-based multiprocessor systems
-   a reduced role for workspace management
-   the addition of new capabilites described in the extensions directory
-   making interactive mode an explicit choice


#Currently Supported Platforms

-   Linux
-   Darwin/OSX


#Getting the Source

You can either use *git* to clone the repository or you can download a zipped 
version of the source code as provided on GitHub.

The source is organised in a simple directory structure


Entry          | Contents                                   
-------------- | ---------------------------------------- 
README.md      | The file you are currently reading 
bin            | Optional directory for Nial executables
docs           | Documentation for QNial
examples       | Some examples of Nial code
modules        | Some examples of extensions that could be added
niallib        | Library of common Nial code
src            | The source code and package builder
testing        | Test code for Nial
tools          | Miscellaneous support tools
tutorial       | Tutorials for Nial


#Building Nial

Initially Nial is built from source by compiling the supplied distribution. This
is a working version of Nial with a core set of functionality based on standard
features of both Linux and OSX. 

Once that version of Nial is built it is possible to build a specialised 
version of Nial using the package builder. The package builder is itself 
written in Nial and allows you to add packages or remove packages from the initial
build.  

The distributed source code assumes that you have downloaded the distribution
to your home directory so that you have a directory *QNial7* in your home 
directory and the Nial library can be found at 

*$HOME/QNial7/niallib*.

Once you have built Nial you can change this by editing the source and using the 
package builder.   

Nial can be built either as a 32 bit system or as a 64 bit system. This is controlled 
by options set in the *CMakeLists.txt* file in the source directory. You can also
set a fast math option which sets the compiler flags appropriately.

The distribution includes binary 64-bit versions of the executable interpreter 
for OSX and Linux in the bin directory. THese builds include the extensions 
described in the extensions directory.

##Requirements

The build process uses CMake and requires that you have installed this. You can use
either the command line version of CMake or the GUI.

The GNU readline library is also required. This should be present by default on OSX but
may need to added to Linux distributions.

Both the CLANG compiler and GCC can be used to build Nial so that you will need
to have installed one of these compilers. On OSX you should install *XCode*, on Linux
use the appropriate package manager for your distribution. CLANG appears to 
generate slightly faster code on Linux.
  

##Building the Distribution

Nial should be built in a separate directory from the source directory
as recommended for cmake.

From the command line Nial can be built with the following commands

     cd QNial7
     mkdir build
     cd build
     cmake ../src
     make

For CLANG use the commands

     cd QNial7
     mkdir build
     cd build
     CC=clang cmake ../src
     make

The output of this will be an executable file *Nial*. To verify that Nial has been
successfully built you can run it interactively as follows

     ./Nial -i 
     sum count 1000
     2*tell 20
     ....
     bye

Once you are happy that Nial is working you should copy Nial
to an appropriate directory on your executable path. An example of this, assuming 
you are using the bash shell, would be

     cp Nial ../bin
     export PATH=$PATH:$HOME/QNial7/bin

Note that the export line should then be added to the appropriate file e.g.
*$HOME/.bashrc* to simplify further use.

If you decide to use one of the provided executables copy it into the bin directory.

# Further Details

There is additional material available at the Wiki page.