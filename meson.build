project(
    'spanmath',
    'cpp',
    default_options: ['cpp_std=c++20'],
    version: '0.1.0',
)

spanmath_dep = declare_dependency(
    include_directories: include_directories('include'),
)

if get_option('tests')
    subdir('tests')
endif
