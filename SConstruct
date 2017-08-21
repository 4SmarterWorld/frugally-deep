import fnmatch
import sys
import os

source_files = []
for root, dirnames, filenames in os.walk('src'):
    for filename in filenames:
        if fnmatch.fnmatch(filename, '*.cpp'):
            source_files.append(str(os.path.join(root, filename)))

build_dir = 'build'

# Windows
if sys.platform[:3] == 'win':
    build_dir = 'build_win'
    env = Environment(tools = ['mingw'])

# Linux etc.
else:
    env = Environment(CXX='clang++')

VariantDir(build_dir, 'src', duplicate=0)
source_files = [s.replace('src', build_dir, 1) for s in source_files]
env.Append(LIBS=['opencv_core', 'opencv_imgproc',
                 'opencv_highgui',
                 'pthread',
                 'boost_filesystem', 'boost_system'])
env.Append(CPPPATH = ['include'])
env.Append(CXXFLAGS='-std=c++14 -O3 -Wall -Wextra -pedantic -Wshadow -Werror -Weffc++ -Wconversion -Wsign-conversion -Wctor-dtor-privacy -Wreorder -Wold-style-cast')
env.Program(target='release/frugally_deep_main', source=source_files)