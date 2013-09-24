#!/usr/bin/python
# -*- coding: ascii -*-

from setuptools import setup, find_packages

__version__ = "1.0.1"

setup(
    name         = "allegrordf",
    version      = __version__,
    author       = "Alcides Fonseca",
    author_email = "me@alcidesfonseca.com.com",
    url          = "http://alcidesfonseca.com/",
    download_url = "http://cheeseshop.python.org/pypi/allegrordf/%s" % __version__,
    description  = "RDFLIB Allegro bindings",
    license      = "LGPL",
    platforms    = ["Platform Independent"],
    classifiers = ["Programming Language :: Python",
                   "License :: OSI Approved :: BSD License",
                   "Topic :: Software Development :: Libraries :: Python Modules",
                   "Operating System :: OS Independent",
                   "Natural Language :: English",
                   ],
   packages = find_packages(exclude=["test"]),
   install_requires = ['pycurl', 'python-cjson'],
   test_suite = 'unittest2.collector',
)
