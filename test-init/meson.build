executable(
    'init',
    'main.c',
    dependencies: hydrogen,
    include_directories: inc,
    install: true,
    install_dir: '/boot',
    link_args: ['-Wl,--no-dynamic-linker,-z,text'],
    link_whole: libc_static,
    objects: crt0,
)
