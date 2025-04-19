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

add_project_link_arguments('-nostdlib', language: 'c')

inc = [include_directories('public', is_system: true), include_directories('private')]

subdir('public')
subdir('crt')
subdir('libc')
subdir('rtld')

libc_static = static_library(
    'c',
    'libc/start-static.S',
    install: true,
    link_whole: [libc_common, rtld_static],
)

libc_shared = shared_library(
    'c',
    'libc/start-shared.S',
    dependencies: hydrogen,
    install: true,
    link_whole: libc_common,
    link_with: rtld_shared,
)

subdir('test-init')
