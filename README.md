PdfViewer
=========

This is a clone of [PdfViewer](http://qt-apps.org/content/show.php?content=149637) from http://QT-Apps.org

It maintains the scroll position when going to next page or previous page.
It also has keyboard shortcuts for both vertical and horizontal scrolling.

Build and install
=================

go to `qtbuild/` and execute  `amkhlv_build.sh` there


Configuration
=============

The configuration file `~/.config/glad/PdfViewer.conf` should contain lines:

    [ShortcutHandler]
    Shortcuts\1\Action=pdfview_go_previous_page
    Shortcuts\1\Shortcut=P
    Shortcuts\2\Action=pdfview_go_next_page
    Shortcuts\2\Shortcut=N
    Shortcuts\size=10
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

