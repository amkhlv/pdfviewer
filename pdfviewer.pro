TEMPLATE = subdirs
#SUBDIRS = lib app
SUBDIRS = sub_lib sub_app

sub_lib.subdir = lib
sub_app.subdir = app
sub_app.depends = sub_lib
