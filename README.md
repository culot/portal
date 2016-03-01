portal - Front-end to pkg(8)
============================


Table of Contents
-----------------

* [Description](#Description)
* [Frequently Asked Questions](#FAQ)
* [TODO](#TODO)
* [Additional resources](#resources)


<a name="Description"></a>
### Description

portal is a front-end to FreeBSD's package manipulation tool pkg(8).
It presents the user with a text-mode interface which comprises two
main panels: one to browse the list of packages, and the other to
display the description of the currently selected package.

Currently, the application is a MVP (Minimum Viable Product)
which provides only the following features: packages listing
and searching, installation and deinstallation of packages,
and filtering based on the package state.


<a name="FAQ"></a>
### Frequently Asked Questions

* Why does portal display weird characters such as squares instead of arrows?

  Check that your environment supports utf8 (terminal, ...) and that you use
  a font that implements a large set of the available utf8 characters (such
  as DejaVu). If for some reasons you do not use such an environment, you can
  start portal with the __-a__ flag so that only ascii characters would be
  used to draw the user interface.


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
