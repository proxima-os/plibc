subdir('start')

rtld = shared_library(
    'ld64',
    'main.c',
    dependencies: hydrogen,
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    install: true,
    link_args: ['-Wl,--entry=rtld_start', '-Wl,--version-script=' + (meson.current_source_dir() / 'rtld.lds')],
    link_depends: files('rtld.lds'),
    link_with: [rtld_start, libc_static],
    name_prefix: '',
    objects: crt0,
    soversion: 1,
)
