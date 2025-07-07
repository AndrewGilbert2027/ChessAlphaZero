from setuptools import setup, Extension
import pybind11
import os

cpp_args = ['-std=c++17', '-O3']

ext_modules = [
    Extension(
        'chessengine',
        sources=[
            'bindings.cpp',
            'game_logic/ChessBoard.cpp',
        ],
        include_dirs=[
            pybind11.get_include(),
            'game_logic',
        ],
        language='c++',
        extra_compile_args=cpp_args,
    ),
]

setup(
    name='chessengine',
    version='1.0',
    author='Andrew Gilbert',
    description='Python bindings for the C++ chess engine',
    ext_modules=ext_modules,
)
