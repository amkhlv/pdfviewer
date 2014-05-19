PdfViewer
=========

This is a clone of [PdfViewer](http://qt-apps.org/content/show.php?content=149637) from http://QT-Apps.org

Differences with the original version:

* maintains the scroll position when going to next page or previous page

* has keyboard shortcuts for both vertical and horizontal scrolling

* modified bookmarks handling:

    - separate adding and deleting of bookmarks, instead of a single toggle function

    - charhints in the bookmarks menu

* after jumping to an internal link or a bookmark, can come back by pressing Ctrl-R

* instead of automatic reload on document change (which may be problematic with some
versions of LaTeX) we use manual reload on pressing Ctrl-L


Build and install
=================

First need to install build dependencies, at least these:

     aptitude install build-essential libqt4-dev qt4-dev-tools libpoppler-qt4-dev zlib1g-dev libconfig++8-dev

go to `qtbuild/` and execute  `amkhlv_build.sh` there


Configuration
=============

The configuration file `~/.config/glad/PdfViewer.conf` should contain lines:

    [MainWindow]
    ShowMenuBar=true

    [ShortcutHandler]
    Shortcuts\1\Action=pdfview_go_previous_page
    Shortcuts\1\Shortcut=P
    Shortcuts\2\Action=pdfview_go_next_page
    Shortcuts\2\Shortcut=N
    Shortcuts\size=12
    Shortcuts\3\Action=amkhlv_dn
    Shortcuts\3\Shortcut=J
    Shortcuts\4\Action=amkhlv_dn_fast
    Shortcuts\4\Shortcut=Shift+J
    Shortcuts\5\Action=amkhlv_rt
    Shortcuts\5\Shortcut=L
    Shortcuts\6\Action=amkhlv_rt_fast
    Shortcuts\6\Shortcut=Shift+L
    Shortcuts\7\Action=amkhlv_up
    Shortcuts\7\Shortcut=K
    Shortcuts\8\Action=amkhlv_up_fast
    Shortcuts\8\Shortcut=Shift+K
    Shortcuts\9\Action=amkhlv_lt
    Shortcuts\9\Shortcut=H
    Shortcuts\10\Action=amkhlv_lt_fast
    Shortcuts\10\Shortcut=Shift+H
    Shortcuts\11\Action=pdfview_bookmarks_prev
    Shortcuts\11\Shortcut=[
    Shortcuts\12\Action=pdfview_bookmarks_next
    Shortcuts\12\Shortcut=]

__HINT__ : I recommend to drag the menu bar (with the mouse) to the left side of the window, so it becomes horizonthal. 
 
