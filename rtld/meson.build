# rtld startup code needs to be built with different compile flags than the rest, put it in a separate static lib
rtld_start = static_library(
    'rtld-start',
    'start/start.c',
    'start/start.S',
    build_by_default: false,
    c_args: ['-ffat-lto-objects', '-ffreestanding', '-fno-stack-protector'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
)

rtld_static = static_library(
    'rtld',
    'main.c',
    build_by_default: false,
    c_args: ['-ffat-lto-objects'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    link_whole: rtld_start,
)

rtld_shared = shared_library(
    'ld64',
    dependencies: hydrogen,
    install: true,
    link_args: ['-Wl,--version-script=' + (meson.current_source_dir() / 'rtld.lds')],
    link_depends: files('rtld.lds'),
    link_with: libc_common,
    link_whole: rtld_static,
    name_prefix: '',
    objects: crt0,
    soversion: 1,
)
