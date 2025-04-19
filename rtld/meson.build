subdir('start')

script = files('rtld.lds')[0]

rtld_static = static_library(
    'rtld',
    build_by_default: false,
    c_args: ['-ffat-lto-objects'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    link_whole: rtld_start,
    pic: true,
)

rtld_shared = shared_library(
    'ld64',
    '../libc/start-static.S',
    'main.c',
    dependencies: hydrogen,
    install: true,
    link_args: ['-Wl,--version-script=' + script.full_path(), '-Wl,-z,text'],
    link_depends: script,
    link_with: libc_common,
    link_whole: rtld_static,
    name_prefix: '',
    objects: crt0,
    soversion: 1,
)
