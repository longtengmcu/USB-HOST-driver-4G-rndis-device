Import('RTT_ROOT')
from building import *

# get current directory
cwd = GetCurrentDir()

# The set of source files associated with this SConscript file.
src = Glob('common/*.c')
src += Glob('mqtt/*.c')
src += Glob('mqttclient/*.c')
src += Glob('network/*.c')
src += Glob('platform/RT-Thread/*.c')

path = [cwd + '/common']
path += [cwd + '/mqtt']
path += [cwd + '/mqttclient']
path += [cwd + '/network']
path += [cwd + '/platform/RT-Thread']

if GetDepend(['KAWAII_MQTT_NETWORK_TYPE_TLS']):
    src += Glob('network/mbedtls/library/*.c')
    src += Glob('network/mbedtls/wrapper/*.c')
    path += [cwd + '/network/mbedtls/wrapper']
    path += [cwd + '/network/mbedtls/include']
    path += [cwd + '/network/mbedtls/include/mbedtls']

if GetDepend(['KAWAII_MQTT_LOG_IS_SALOF']):
    src += Glob('common/log/*.c')
    src += Glob('common/log/arch/rt-thread/*.c')
    path += [cwd + '/common/log']

if GetDepend(['PKG_USING_KAWAII_MQTT_TEST']):
    src += Glob('test/*.c')

group = DefineGroup('kawaii_mqtt',src , depend = ['PKG_USING_KAWAII_MQTT'], CPPPATH = path)

Return('group')
