crt_args = []

if get_option('debug')
    crt_args += '-g'
endif

foreach name : ['crt1', 'crti', 'crtn']
    set_variable(name, custom_target(
        name,
        build_by_default: true,
        command: compiler.cmd_array() + ['@INPUT@', '-o', '@OUTPUT@', '-c'] + crt_args,
        input: files(name + '.S'),
        install: true,
        install_dir: get_option('libdir'),
        install_tag: 'runtime',
        output: name + '.o',
    ))
endforeach
