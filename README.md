portal - Front-end to pkg(8)
============================


Table of Contents
-----------------

* [Description](#Description)
* [Frequently Asked Questions](#FAQ)
* [TODO](#TODO)


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

* the packages version should be displayed (current and available), and
  upgradable packages should be identified as such
* a new filtering rule should be added to only show packages with
  pending actions (ie those who are marked for installation or
  removal)
* portal should use the libpkg directly instead of calling pkg(8)
