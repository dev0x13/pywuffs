import os
import sys
import re

project = 'pywuffs'
copyright = '2023, pywuffs authors'

master_doc = 'index'

extensions = ['sphinx.ext.autodoc', 'sphinx_epytext', 'sphinx.ext.coverage', 'sphinx.ext.napoleon', 'sphinx.ext.autosummary']

exclude_patterns = []

html_theme = 'sphinx_rtd_theme'
