from distutils.core import setup
import py2exe, glob

setup(windows=["SpeedRisk.py"],
        data_files=[
            ("images",glob.glob("images\\*.png")),
            ("images",glob.glob("images\\*.jpg")),
            ("images",["images\\manifest"]),
            (".",["config.ini"]),
        ],
        includes=['Numeric'],
     )

