rtld_sources += files(
    'object.c',
    'rtld.c',
    'tls.c',
)

rtld_start_sources += files('start.c')
rtld_shared_sources += files('main.c')

rtld_start = static_library(
    'rtld-start',
    build_by_default: false,
    c_args: ['-ffat-lto-objects', '-ffreestanding', '-fno-stack-protector'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    pic: true,
    sources: rtld_start_sources,
)

script = files('rtld.lds')[0]

rtld_common = static_library(
    'rtld',
    build_by_default: false,
    c_args: ['-ffat-lto-objects'],
    gnu_symbol_visibility: 'hidden',
    include_directories: inc,
    link_whole: rtld_start,
    pic: true,
    sources: rtld_sources,
)

rtld_shared = shared_library(
    'ld64',
    dependencies: hydrogen,
    include_directories: inc,
    install: true,
    link_args: ['-Wl,--version-script=' + script.full_path(), '-Wl,-z,text'],
    link_depends: script,
    link_with: libc_common,
    link_whole: rtld_common,
    name_prefix: '',
    objects: crt1,
    sources: libc_static_sources + rtld_shared_sources,
    soversion: 1,
)
