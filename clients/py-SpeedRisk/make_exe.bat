rd /S /Q build
rd /S /Q dist

python setup.py py2exe -b 2 -O2 -i Numeric
