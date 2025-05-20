project(
    'plibc',
    'c',
    default_options: ['c_std=gnu11', 'warning_level=2'],
    license: 'MIT',
    license_files: 'LICENSE',
    meson_version: '>=1.4.0',
    version: '0.1.0',
)

compiler = meson.get_compiler('c')
hydrogen = compiler.find_library('hydrogen')

add_project_arguments('-D__PLIBC_BUILDING_PLIBC', '-D_PROXIMA_SOURCE', '-Wno-unused-parameter', language: 'c')
add_project_link_arguments('-nolibc', '-nostartfiles', '-Wl,-Bsymbolic', language: 'c')

inc = [include_directories('public', is_system: true), include_directories('private')]

libc_sources = []
libc_sublibs = []
libc_shared_sources = []
libc_static_sources = []
rtld_sources = []
rtld_start_sources = []

subdir('arch' / host_machine.cpu_family())
subdir('public')
subdir('libc')
subdir('rtld')

libc_static = static_library(
    'c',
    install: true,
    link_whole: [libc_common, rtld_static],
    sources: libc_static_sources,
)

libc_shared = shared_library(
    'c',
    dependencies: hydrogen,
    install: true,
    link_whole: libc_common,
    link_with: rtld_shared,
    sources: libc_shared_sources,
)

subdir('test-init')
