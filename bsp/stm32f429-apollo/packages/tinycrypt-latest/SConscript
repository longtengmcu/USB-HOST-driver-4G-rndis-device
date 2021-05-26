# RT-Thread building script for component

from building import *

cwd = GetCurrentDir()
src = Glob('src/*.c') + Glob('src/*.cpp')
CPPPATH = [cwd + '/include']

if GetDepend(['TINYCRYPT_USING_AES_SAMPLE']):
    src += Glob('samples/*.c')

group = DefineGroup('TinyCrypt', src, depend = ['PKG_USING_TINYCRYPT'], CPPPATH = CPPPATH)

Return('group')
