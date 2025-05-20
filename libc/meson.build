subdir('assert')
subdir('ctype')
subdir('cxa')
subdir('dirent')
subdir('errno')
subdir('fcntl')
subdir('grp')
subdir('locale')
subdir('musl-generic-math')
subdir('pwd')
subdir('ryu')
subdir('setjmp')
subdir('signal')
subdir('stdio')
subdir('stdlib')
subdir('string')
subdir('sys')
subdir('termios')
subdir('time')
subdir('unistd')
subdir('utime')

libc_sources += files(
    'config-file.c',
    'start.c',
)

libc_common = static_library(
    'c-common',
    build_by_default: false,
    c_args: ['-ffat-lto-objects', '-ffreestanding', '-Wno-prio-ctor-dtor'],
    gnu_symbol_visibility: 'hidden',
    implicit_include_directories: false,
    include_directories: inc,
    link_whole: libc_sublibs,
    pic: true,
    sources: libc_sources,
)
