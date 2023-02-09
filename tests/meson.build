gtest_dep = dependency('gtest')
gtest_main_dep = dependency('gtest_main')

executable(
    'spanmath_test',
    [
        'spanmath_test.cpp',
    ],
    install: true,
    dependencies: [gtest_dep, gtest_main_dep, spanmath_dep],
)
