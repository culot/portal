portal - Front-end to pkg(8)
============================


Table of Contents
-----------------

* [Description](#Description)
* [TODO](#TODO)
* [Additional resources](#resources)


<a name="Description"></a>
### Description

portal is a front-end to FreeBSD's package manipulation tool pkg(8).
It presents the user with a text-mode interface which comprises two
main panels: one to browse the list of packages, and the other to
display the currently selected package's comment line as found in
the port's Makefile, together with its longer comment as found in
the port's pkg-descr file.

Three different modes are currently supported:

1. *browse* mode, with which the user sees the list of all packages
   without any filtering
2. *search* mode, which allows to enter a string that will be used
   to search the packages repository
3. *filter* mode, in which various filters can be toggled on or off
   to narrow the list of packages to be displayed


<a name="TODO"></a>
### TODO

This is the list of items that should be worked on for future releases

* portal should be made configurable using a ~/.portal.conf file
* portal should use the libpkg directly instead of calling pkg(8)


<a name="resources"></a>
### Additional resources

* The Git repository of [portal is hosted on GitHub][1]
* In order to share your thoughts about portal, you can join the
  __#portal__ irc channel on EFnet.
* If you hit a bug when using portal, you can submit an issue in the
  [portal issue tracker][2].

[1]: https://github.com/culot/portal
[2]: https://github.com/culot/portal/issues
