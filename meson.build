project(
    'plibc',
    'c',
    default_options: ['c_std=gnu11', 'warning_level=2'],
    license: 'MIT',
    license_files: 'LICENSE',
    meson_version: '>=1.1.0',
    version: '0.1.0',
)

compiler = meson.get_compiler('c')
hydrogen = compiler.find_library('hydrogen')

add_project_arguments('-Wno-unused-parameter', language: 'c')
add_project_link_arguments('-nostdlib', language: 'c')

inc = include_directories('include', is_system: true)

subdir('include')
subdir('crt')
subdir('libc')
subdir('rtld')

libc_static = static_library(
    'c',
    install: true,
    link_whole: [libc_common, rtld_static],
)

libc_shared = shared_library(
    'c',
    dependencies: hydrogen,
    install: true,
    link_whole: libc_common,
    link_with: rtld_shared,
)

subdir('dummy')
