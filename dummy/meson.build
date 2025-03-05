foreach dummy : ['m', 'rt', 'pthread', 'xnet']
    both_libraries(
        dummy,
        'dummy.c',
        c_args: ['-ffreestanding'],
        gnu_symbol_visibility: 'hidden',
        include_directories: inc,
        install: true,
        link_with: libc_shared,
    )
endforeach
